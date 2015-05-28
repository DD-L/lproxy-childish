/**
 *    web.cpp 
 *    Web Server 与 Client 之间交换数据
 */
#include <cstring>
#include <cstdlib> 
#include "log.h"
#include "web.h"
#include "charcodecast.h"
extern CCC ccc;
/*
 *取出header中的 Content-Length字段 
 */
int Web::get_data_len(char *data) {
	char *p = NULL;
	char *pend = NULL;
	p = strstr(data, "Content-Length:");
	if (p==NULL) return -1;
	p += 15;
	pend = strstr(p, "\r\n");
	if (pend==NULL) return -1;
	int l = pend - p;
	char *len = NULL;
	len = (char *)malloc(l);
	memcpy(len, p, l);
	return atoi(len);
}
/* 
 * 取出header 
 */
int Web::get_server_header(Sock& sock_web,
                      char **header_data,int *header_len,
                      char **other_data,int *other_len) {

	char *data;
	int read_len = sock_web.read_from_net((unsigned char **)&data);

	if (read_len == 0) {
		write_log(LOGINFO,"server is closed");
		return -1;
	}
	else if (read_len == -1) {
		write_log(LOGERR,"read_server_error!");
		return -1;
    	}
	char *p = NULL;
	p = strstr(data, "\r\n\r\n");
	if (p == NULL) return -1;

	*header_len = p - data;
	*header_len += 4;
	*other_len = read_len - *header_len;
	*header_data = (char *)malloc(*header_len + 1);
	*other_data = (char *)malloc(*other_len + 1);
	memset(*header_data, '\0', *header_len + 1);
	memset(*other_data, '\0', *other_len + 1);
	memcpy(*header_data, data, *header_len);
	memcpy(*other_data, data+*header_len, *other_len);
	free(data);
    	return 1;
}

/**
 * Web 初始化， 先向 WebSever发送一次请求
 */
Web::Web(Sock& sock_web, unsigned char* str_request, size_t len) 
                              throw (SocketException) {
    try {
        sock_web.write_from_net(str_request, len);
    }
    catch (SocketException& ex) {
        throw; // 原样抛出
    }
}

/*
 * 连接web_server 并初始化后，
 * 接收数据，分析数据， 交换数据
 */
void Web::operator() (Sock& sock_web, Sock& sock_c) 
                    throw (SocketException) { 
	
        char *data = NULL;
	char *header_data = NULL;
	int data_len;
	int header_len;
	
	int errcode = get_server_header(sock_web, &header_data, 
    				        &header_len, 
                                        &data, &data_len);
   
	if (errcode != 1) {
		write_log(LOGERR,"get server header packet error");
		sock_web.close();
		sock_c.close();
		if (data) free(data);
		if (header_data) free(header_data);
		return;
	}
#ifdef _DEBUG
    CCC ccc;
    cout << ccc("\nWebServer 返回的数据:") << endl;
    cout << "header_data = \n" <<  ccc(header_data) << endl;
    cout << "header_len = " << header_len << endl;
    cout << "data = \n" << ccc(data) << endl;
    cout << "data_len = " << data_len << endl;
#endif
        int content_length = get_data_len(header_data);
	sock_c.write_from_net((unsigned char*)header_data, header_len);
	sock_c.write_from_net((unsigned char*)data, data_len);
	while(data_len < content_length || content_length == -1) {
		int read_len 
                    = sock_web.read_from_net((unsigned char **)&data);
		if (read_len == 0) {
			write_log(LOGINFO,"server is closed");
			break;
          	}
          	else if (read_len == -1) {
			write_log(LOGERR,"read data from server error!");
			break;
          	}
		data_len += read_len;
		sock_c.write_from_net((unsigned char*)data, read_len);
	}
#ifdef _DEBUG
	//写向客户端的数据
	cout << "\n\nData write to client:\n" << data << "\n\n";
#endif
	sock_c.close();
	sock_web.close();
	if(data) free(data);
	if (header_data) free(header_data);
}
