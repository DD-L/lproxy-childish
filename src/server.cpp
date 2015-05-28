
// server.cpp
#include "server.h"
#include "except.h"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include "sock.h"
#include "log.h"
#include "thread.h" 
#include <unistd.h>

/*
   int _port = 2222;
   string _client_ip_list = "";
   bool _run_server = true;
*/ 


Server::Server(int port, string client_ip_list)
                : _port(port), _client_ip_list(client_ip_list),
                  _run_server(true) {}

void Server::start() {
    // 一个服务器 socket 对象， 一个 客户端 socket 对象
    Sock sock_s(SERVER, _port), sock_c(CLIENT);
    
    // 打开指定的端口 并 绑定本机 和 监听
    try {
        sock_s.open_socket();
    }
    catch (SocketException& ex) {
        write_log(LOGERR, ex.what());
        return;
    }
    // 绑定等系列操作成功
    write_log(LOGINFO, "Open port success!");

    // 循环 accept 客户端
    while (_run_server) {
        struct sockaddr_in client;
        socklen_t client_len = sizeof (client);
        
        try {
            // 接收 客户端
            sock_c = sock_s.accept_client(&client, &client_len);
            
            // 如果客户端不在 _client_ip_list 中
            if (_client_ip_list != "" &&
                (_client_ip_list.find(inet_ntoa(client.sin_addr)) 
                                                 == string::npos)) {
            	// alert
            	write_log(LOGALERT, inet_ntoa(client.sin_addr)
            				+ string(", refuse request!"));
            	sock_c.close();
            	continue;
            } 
            
            // 有客户端接入
            write_log(LOGINFO, 
                        inet_ntoa(client.sin_addr)
                        + string(" accept client success!"));

            // 创建用于读取 client 数据的线程
            Thread *pthread = new ReadClientThread(sock_c);
            pthread->start(); // 线程分离， 并行回收资源 
        }
        catch (SocketException& ex) {
            write_log(LOGERR, ex.what());            
        }
        catch (ThreadException& ex) {
            write_log(LOGERR, ex.what());
        }
        catch (HTTPException& ex) {
            write_log(LOGERR, ex.what());
        }
        /*
        catch (ServerException& ex) {
            write_log(LOGERR, ex.what());
        }
        */
        catch (...) {
            stop();
        }
    }
}

// 
bool Server::stop() {
    _run_server = false;
    return true;
}

