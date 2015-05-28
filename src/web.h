#ifndef _WEB_H
#define _WEB_H

// Web Server & Client 之间交换数据
#include "sock.h"
#include "except.h"
class Web {
    private:
        int get_data_len(char* data);
        int get_server_header(Sock& sock_web, char** header_data, 
                                int* header_len, char** other_data, 
                                int* other_len);
    public:
        Web(Sock& sock_web, unsigned char* str_request, size_t len)
                            throw (SocketException);        
        void operator() (Sock& sock_web, Sock& sock_c) 
                            throw (SocketException);
};
#endif
