/**
 *作用：分析客户端连接。
 *      读取client发送的请求，判断是否为合法的http请求。
 *参数：data     ：client发送的字符. 
 *       str     :发送到WEB SERVER的请求字符。
 *       host    :从get_str取出的host地址
 *       port    :端口。  
 */
 
#ifndef _HTTP_H
#define _HTTP_H

#include "except.h"
// Http 请求数据分析, 并把结果通过指针带出来
class Http{
    public:
        int operator() (char* data, char** str, char** host,
                        char** port, char** path)
                        throw (HTTPException, SocketException);
        void err_codes(int err_id, char** str);// 得到错误Http状态码信息
        static int strnicmp(const char* s1, const char* s2, size_t n);
    private:
        int analyse_url_str(char* url, char** host, 
                            char** port, char** path);
        int get(char* get_str, char** str, char** host, 
                char** port, char** path);
        int post(char* post_str, char** str, char** host,
                char** port, char** path);
        int conn(char* conn_str, char** str, char** host, 
                char** port, char** path);
        static char toi(char c);
};
#endif
