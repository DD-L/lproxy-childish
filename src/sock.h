/**
 *  sock.h
 */ 
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <string>
#include "except.h"
#ifndef _SOCK_H
#define _SOCK_H

typedef int SOCKET;
enum SockType { CLIENT, SERVER, WEBSERVER };

class Sock {
    public:
        Sock(SockType sock_type, int port = 0);
        Sock(const Sock& that);
        Sock& operator= (const int& fd);   
        const SOCKET fd(); 
        // 设置端口
        void set_port(int port);
        // 打开端口 
        void open_socket() throw (SocketException);
        // 连接到 远程计算机
        void open_connect(string host, int port = 80) 
                        throw (SocketException);
        // 接受连接 返回客户端 SOCKET， 负数为错误
        SOCKET accept_client(struct sockaddr_in *client, 
                            socklen_t* client_len) 
                            throw (SocketException);
        // 从相应的socket都取数据
        int read_from_net(unsigned char** read_str)
        		throw (SocketException);
        // 写入数据到 socket 中
        int write_from_net(unsigned char* write_str, 
                           int write_str_len) 
                           throw (SocketException);
        // 向client入 错误信息
	int write_error(int err_id) throw(); // 用在catch块，什么都不能抛
        void close();
        ~Sock();
        const SockType type() {
            return _socktype;
        }

    private:
        const SockType _socktype;
        int _port;
        SOCKET _fd;
};
#endif // _SOCK_H
