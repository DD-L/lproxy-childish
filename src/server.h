/**
 *  server.h 接收client url 请求， 连接 webserver
 */ 
#ifndef _SERVER_H
#define _SERVER_H
#include <string>
using namespace std;
class Server {
    private:
        int _port;
        string _client_ip_list;
        bool _run_server;
    public:
        Server(int port = 2222, string client_ip_list = "");
        void start();
        bool stop();
};
#endif //_SERVER_H
