// thread.cpp

#include <cstdlib>
#include <pthread.h>
#include <cstring>
#include "thread.h"
#include "http.h"
#include "web.h"
#include "log.h"
#include "charcodecast.h"
extern CCC ccc;
//////////////////////////////////////////
// Thread 类实现
void Thread::start() throw (ThreadException) {
    pthread_t tid;
    CCC ccc(30);
    if (pthread_create(&tid, NULL, run, this))
            throw ThreadException(ccc("启动线程失败!"));
}

// 线程过程 （调用 线程处理函数）
void* Thread::run(void* arg) {
    // 设置线程分离状态
    pthread_detach(pthread_self());
    // 调用 线程处理 虚函数
    static_cast<Thread *> (arg)->run();
    return NULL;
}


////////////////////////////////////////////
// ReadClientThread 类实现
ReadClientThread::ReadClientThread(Sock sock) 
        throw (ThreadException) : _sock_c(sock) {
            if (_sock_c.type() != CLIENT) {
            	CCC ccc(100);
                throw ThreadException(ccc("线程对象创建失败！"
                    "Sock 类型不匹配，必须为 CLIENT"));
            }
}

// 必须由堆对象调用
void ReadClientThread::run() {
    Sock sock_web(WEBSERVER);
    char *data = NULL, *str = NULL, *host = NULL,
         *port = NULL, *path = NULL;
    try {
        // 读取 Client 请求
        int read_len = 0;
        read_len = _sock_c.read_from_net((unsigned char**)&data);
        CCC ccc;
#ifdef _DEBUG
	
        cout << "\nThread::run: New Client fd = " << _sock_c.fd() << endl;
        cout << ccc("客户端请求数据长度:") << "strlen(data) = " << strlen(data)
             << ccc("\n请求数据: \n") << ccc(data) << endl;
        cout << ccc("分析请求数据... ") << endl;
#endif
        // 分析请求数据
        Http analyzer_http_string;
        analyzer_http_string(data, &str, &host, &port, &path);

        if (atoi(port) == 0 || atoi(port) > 65000)
            throw HTTPException(ccc("得到的port值异常"));
#ifdef _DEBUG
        cout << ccc("分析结束，得到 host 和 port") << endl;
        cout << "str = \n" << ccc(str) << endl;
        cout << "host = " << host << endl;
        cout << "port = " << port << endl;
        cout << "path = " << path << endl;
        cout << ccc("开始连接 Web Server, 发送数据...") << endl;
#endif

        // 设置 webServer 端口 并 连接到 webServer
        // sock_web.set_port(atoi(port));
        sock_web.open_connect(host, atoi(port));
        
        // WebServer 与 Client 之间交换数据 
        // 初始化 WebServer
        Web web_client(sock_web, (unsigned char*)str, strlen(str));
        // 交换数据
        web_client(sock_web, _sock_c);
#ifdef _DEBUG
        cout << ccc("\n转发完成, 回收资源, 线程结束\n\n") << endl;
#endif
    }
    catch (SocketException& ex) {
        write_log(LOGERR, ex.what());
        if (sock_web.fd() == -1 || sock_web.fd() == 0)
            _sock_c.write_error(404);
        else {
            _sock_c.write_error(403);
        }
    }
    catch (HTTPException& ex) {
        write_log(LOGERR, ex.what());
        _sock_c.write_error(403);
    }
    
    // 回收资源
    if (data) {
        free(data); data = NULL;
    }
    if (str) {
        free(str);  str = NULL;
    }
    if (host) {
        free(host); host = NULL;
    }
    if (port) {
        free(port); port = NULL;
    }
    if (path) {
        free(path); path = NULL;
    }
    _sock_c.close();
    delete this;
    pthread_exit((void *)-1);
    return;
}
