// Sock.cpp
#include <cstring>
#include <netdb.h>
#include <cstdlib>
#include "sock.h"
#include "log.h"
#include "except.h"
#include "http.h"
#include "charcodecast.h"
extern CCC ccc;
extern int h_errno;

Sock::Sock(SockType sock_type, int port) 
            : _socktype(sock_type), 
            _port(port), _fd(-1) {}
Sock::Sock(const Sock& that) 
            : _socktype(that._socktype), 
            _port(that._port),
            _fd(that._fd) {}

Sock& Sock::operator= (const int& fd) {
    _fd = (SOCKET)fd;
    return *this;
}

const SOCKET Sock::fd() {
    return _fd;
}

// 设置端口
void Sock::set_port(int port) {
    if (_socktype == CLIENT) {
        write_log(LOGALERT, ccc("当前sock类型为 CLIENT, 端口无需设置！")); 
        return;
    }
    _port = port;
}
// 打开端口 
void Sock::open_socket() throw (SocketException) {
    
    if (_socktype != SERVER) {
        write_log(LOGALERT, ccc("打开端口操作只适用于类型为 SERVER 的 sock!"));
#ifdef _DEBUG
        write_log(LOGALERT, 
                ccc("Sock::open_socket只适用于类型为 SERVER 的 sock!"));
#endif
        return;
    }

    _fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd == -1) {
        throw SocketException("Server: Open socket ERROR!");
    }

    struct sockaddr_in local;
    local.sin_family = AF_INET;
    local.sin_addr.s_addr = htonl(INADDR_ANY); // local IP
    local.sin_port = htons((uint16_t)_port);

    // 解决重启地址端口被占用问题
    int reuseaddr = 1;
    setsockopt(_fd, SOL_SOCKET, SO_REUSEADDR, 
                &reuseaddr, sizeof (reuseaddr));

    // 绑定
    if (bind(_fd, (struct sockaddr*)&local, sizeof (local)) != 0) {
        throw SocketException("Server: bind failed!");
    }

    // 监听
    if (listen(_fd, 100) != 0) {
        throw SocketException("Server: listening failed!");
    }

    
}

// 连接到 远程计算机 返回 fd_web
void Sock::open_connect(string host, int port) // 默认为 80 
                throw (SocketException) {
    if (_socktype != WEBSERVER) {
        write_log(LOGALERT, 
                ccc("连接Web Server的类型必须为Sock(WEBSERVER)"));
#ifdef _DEBUG
        write_log(LOGALERT, 
                ccc("Sock::open_connet只适用于类型为 WEBSERVER 的 sock!"));
#endif
        return;
    }

    _port = port;
    
    struct sockaddr_in remote;
    struct hostent *hp;
    // unsigned int addr;
    
    _fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (_fd == -1 || _fd == 0) {
        throw SocketException("WebServer: "
                "socket error while opening connet.");
    }

    if(inet_addr(host.c_str()) == INADDR_NONE) { // -1
        hp = gethostbyname(host.c_str());
        if(hp == NULL) {
            close();
            throw SocketException("WebServer: "
                    "get host ip list error while opening connect.");
        }
        remote.sin_addr.s_addr = *((unsigned long*)hp->h_addr);
    }
    else {
        remote.sin_addr.s_addr= inet_addr(host.c_str());
    }
    
    remote.sin_family = AF_INET;
    remote.sin_port = htons((uint16_t)_port);
    if(connect(_fd, (struct sockaddr*)&remote, sizeof(remote))) {
        close();
        throw SocketException("WebServer: open connect error");
    }

}

// 接受连接 返回客户端 SOCKET， 负数为错误
SOCKET Sock::accept_client(struct sockaddr_in *client, 
                    socklen_t* client_len) throw (SocketException) {
    SOCKET fd_c = -1;
    fd_c = accept(_fd, (struct sockaddr *)client, client_len);
    if (fd_c == -1 || fd_c == 0) {
        throw SocketException("accpet from client ERROR!");
    }
    return fd_c;
}

// 从相应的socket读取数据， 返回读取的长度
int Sock::read_from_net(unsigned char** read_str) 
            throw (SocketException) {
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    fd_set fdread;
    int ret = -1;

    int recv_len = 0;
    int tmp_len = 2046; //
    char tmp[tmp_len];
    memset(tmp, 0, tmp_len);

    FD_ZERO(&fdread);
    FD_SET(_fd, &fdread);
    
    CCC ccc;
    if ((ret = select(_fd + 2/*0*/, &fdread, NULL, NULL, &tv))
            == -1) {
        // 出现错误
        throw SocketException(ccc("在读取数据报文时 SELECT 检测到异常!"
                "该异常导致线程终止!")); 
    }
    
    if (ret > 0) {
        if (FD_ISSET(_fd, &fdread)) {
            recv_len = recv(_fd, tmp, tmp_len - 1, 0);
            if (recv_len == -1 || recv_len == 0
                    || recv_len > tmp_len) {
                if (_socktype == CLIENT) {
                    throw SocketException("read url from client error");
                }
                if (_socktype == WEBSERVER)
                    throw SocketException("read data from"
                            " webServer error");
                throw SocketException("read data error");         
            }
        }
    }
    
    if (_socktype == CLIENT) { // url 太长; 请求数据太长 
        if (recv_len > 2000) { // < 1450
            write_log(LOGERR, "client url is too long");
            write_log(LOGERR, tmp);
            throw SocketException();
        }
    }
    else if (_socktype == WEBSERVER) { // 从webServer端的读取数据检测处理
        // 
    }
/*
#ifdef _DEBUG
    cout << "Sock::read_from_net 当前fd: " << _fd << endl;
    cout << "Sock::read_from_net 读取的数据:" << tmp << endl;
#endif
*/
    *read_str = (unsigned char *)malloc(recv_len);
    memcpy(*read_str, tmp, recv_len);
    return recv_len;
}
// 写入数据到 socket 中, 返回写入的长度
int Sock::write_from_net(unsigned char* write_str, 
                            int write_str_len) 
                            throw (SocketException) {
    struct timeval tv;
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    fd_set fdwrite;
    int ret, write_len = 0, idx = 0;
    CCC ccc;
    // 循环写入数据
    while (write_str_len) {
        FD_ZERO(&fdwrite);
        FD_SET(_fd, &fdwrite);
        if ((ret = select(_fd + 2, NULL, &fdwrite, NULL, &tv))
                == -1) {
            close();
            throw SocketException(ccc("写入数据时 SELECT 异常! "
                    "该异常导致线程终止!"));
        }
        if (ret > 0) {
            if (FD_ISSET(_fd, &fdwrite)) {
                write_len = send(_fd, write_str + idx, 
                                    write_str_len, 0);
                if (write_len <= 0) { 
                    close();
                    if (_socktype == WEBSERVER)
                        throw SocketException("write postdata to web "
                                "error!");
                    if (_socktype == CLIENT)
                        throw SocketException("write socket to "
                                                "client error!");
                    throw SocketException("write socket error");
                }
                write_str_len -= write_len;
                idx += write_len;
            }
        }
    }
    return idx;
}

// 向client入 错误信息
int Sock::write_error(int err_id) throw() {
    if (_socktype != CLIENT) {
        write_log(LOGALERT, ccc("Sock::write_error 只适用于写入到 client 端!"));
        return 0;
    }
    
    char* str = NULL;
    int ret = 0;
    CCC ccc;
    try {
        // 得到错误码信息
        Http http;
        http.err_codes(err_id, &str);
#ifdef _DEBUG
        cout << ccc("HTTP错误码:") << err_id << ccc(" 错误信息:\n") << str << "\n\n";
#endif
        if (!str) throw HTTPException(ccc("没有得到HTTP错误码信息!"));

        // 向客户端写入Http错误信息
        ret = write_from_net((unsigned char*)str, strlen(str));
    }
    catch (HTTPException& ex) {
         write_log(LOGERR, string(ex.what()) +
                 " when write HTTP Error Codes to Client.");
    }
    catch (SocketException& ex) {
        write_log(LOGERR, string(ex.what()) + 
                " when write HTTP Error Codes to Client.");
    }
    catch (...) {
        return -1;
    }
    return ret;
}

void Sock::close() {
    if (_fd != -1) {
        ::close(_fd);
        _fd = -1;
    }
}
Sock::~Sock() { // 不能关闭 _fd
    //close();
#ifdef _DEBUG
    //cout << (_socktype == CLIENT) << " Sock::~Sock" << endl;
#endif
}

