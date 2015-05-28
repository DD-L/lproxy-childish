/*
	Description: response http protocol
*/

#include <cstring>
#include <cstdlib> 
#include <cstdio>
#include "http.h"
#include "log.h"
/**
 * Description：分析url字符串，取出host地址。 
 *   URL 形式分类:
 *    1、http://www.baidu.com/a/b.htm
 *    2、http://www.baidu.com:8080/a/b.htm 
 *    3、www.baidu.com:443
 *    4、http://www.baidu.com/
 *    5、ftp://www.baidu.com/
 * 
 */
int Http::analyse_url_str(char* url, char** host, 
                            char** port, char** path) {
    	char* p = NULL;
    	//查找字符串"://" ，移除http://等字符串 
    	p = strstr(url, "://");
    	if (p) {
       		int t = p - url;
        	url += t;
        	url += 3;
	}
    
    	*host = (char *)malloc(strlen(url) + 1);
    	int nport;
    	*path = (char *)malloc(strlen(url) + 1);
    
    	if (sscanf(url, "%[^:/]:%hu%s", *host,
    		 (short unsigned int*)&nport, *path) == 3);
	else if (sscanf(url, "%[^/]%s", *host,*path) == 2)
		nport = 80;
	else if (sscanf(url, "%[^:/]:%hu", *host, (short unsigned int*)&nport)
		 == 2)
		strcpy(*path, "/");
	else if (sscanf(url, "%[^/]", *host) == 1) {
		nport = 80;
		strcpy(*path, "/");
	} 
	else {
		return -1;
	}
	*port = (char *)malloc(6);
	memset(*port, 0, 6);
	//itoa(nport, *port, 10);
	sprintf(*port, "%d", nport);
	return 1;
}

/**
 * 作用： 分析Get 请求字符串。 
 * 参数：get_str :client发送的请求字符。
 *       str     :发送到WEB SERVER的请求字符。
 *       host    :从get_str取出的host地址
 *       port    :端口。 
 * 返回值：负数为错误。 
 */
int Http::get(char* get_str, char** str, 
                char** host, char** port, char** path) {
	//char *method = "GET";
	char* url = NULL;           //url所有字符 
	//char *host;          //host地址 
	//char *port;
	//char *path;          
	char* proto_ver = NULL;     //http协议及版本 
	char* other = NULL;         //其它部分 
	char* p = NULL;    //保存临时位置 

	get_str += 4;
	//找到url部分
	if (strchr(get_str, ' ') == 0) get_str++;
	p = strchr(get_str, ' ');  //寻找空格位置 


	if (!p ) return -1;
	int url_len = p - get_str;
	url = (char *)malloc(url_len + 1);
	memset(url, '\0', url_len + 1);
	memcpy(url, get_str, url_len);
	get_str += url_len;           //get_str向后移动到协议部分。 
	if(analyse_url_str(url, host, port, path) < 0) {
		//if (method) free(method);
		if (url) free(url);
	 	return -1;
	}

	//协议部分
	if (strncmp(get_str, " ", 1) == 0) get_str++;
	p = strstr(get_str, "\r\n");
	if (!p) {
		//if (method) free(method);
		if (url) free(url);
		return -2;
	}
	int proto_len = p - get_str;
	proto_ver = (char *)malloc(proto_len + 1);
	memset(proto_ver, '\0', proto_len + 1);
	memcpy(proto_ver, get_str, proto_len);
	get_str += proto_len;
	
	//其它部分 
	int other_len = strlen(get_str);
	other = (char *)malloc(other_len + 1);
	memset(other, '\0', other_len + 1);
	memcpy(other, get_str, other_len);
     
	//提交给web服务器的请求字符串。 
	//形式如同：GET / http/1.0
	//          等 
	//char *str; 
	int str_len = 3 + 1 + strlen(*path) + 1 + proto_len + other_len; 
	*str = (char *) malloc(str_len + 1);
	memset(*str, 0, str_len + 1);
	int idx = 0;
	memcpy(*str + idx, "GET ", 4);
	idx += 4;
	memcpy(*str + idx, *path, strlen(*path));
	idx += strlen(*path);
	memcpy(*str + idx, " ", 1);
	idx += 1;
	memcpy(*str + idx, proto_ver, proto_len);
	idx += proto_len;
	memcpy(*str + idx, other, other_len);

/*
printf("url = %s\n", url);
printf("proto_ver = %s\n", proto_ver);
printf("other = %s\n", other);
printf("p = %s\n", p);
*/    
	//
	// if (method) free(method);
     
	if (url) free(url);
	if (proto_ver) free(proto_ver);
	if (other) free(other);
	if (p && !strlen(p)) free(p);

	return 1;
}


/**
 * 作用： 分析post请求字符串。 
 * 参数：post_str :client发送的请求字符。
 *       str     :发送到WEB SERVER的数据。
 *       host    :从get_str取出的host地址
 *       port    :端口。 
 * 返回值：负数为错误。
 */
int Http::post(char* post_str, char** str, 
                    char** host, char** port, char** path) {
	//char *method = "post";
	char* url = NULL;           //url所有字符        
	char* proto_ver = NULL;     //http协议及版本 
	char* other = NULL;         //其它部分 
	char* p = NULL;    //保存临时位置 
     
	post_str += 5;
	//找到url部分
	if (strchr(post_str, ' ') == 0) post_str++;
	p = strchr(post_str, ' ');  //寻找空格位置 
	if (!p ) return -1;
	int url_len = p - post_str;
	url = (char *)malloc(url_len + 1);
	memset(url, '\0', url_len + 1);
	memcpy(url, post_str, url_len);
	post_str += url_len;           //get_str向后移动到协议部分。
	 
	if (analyse_url_str(url, host, port, path) < 0) {
		//if (method) free(method);
		if (url) free(url);
		return -1;
	}
        
	//协议部分
	if (strncmp(post_str, " ", 1) == 0) post_str++;
	p = strstr(post_str, "\r\n");
	if (!p) {
		//if (method) free(method);
		if (url) free(url);
		return -2;
     	}
	int proto_len = p - post_str;
	proto_ver = (char *)malloc(proto_len + 1);
	memset(proto_ver, '\0', proto_len + 1);
	memcpy(proto_ver, post_str, proto_len);
	post_str += proto_len;

	int other_len = strlen(post_str);
	other = (char *)malloc(other_len + 1);
	memset(other, '\0', other_len + 1);
	memcpy(other, post_str, other_len);

	//提交给web服务器的请求字符串。 
	//形式如同：POST / http/1.0
	//          等 
	//     char *str; 
	int str_len = 4 + 1 + strlen(*path) + 1 + proto_len + other_len; 
	*str = (char *) malloc(str_len + 1);
	memset(*str, '\0', str_len + 1);
	int idx = 0;
	memcpy(*str + idx, "POST ", 5);
	idx += 5;
	memcpy(*str + idx, *path, strlen(*path));
	idx += strlen(*path);
	memcpy(*str + idx, " ", 1);
	idx += 1;
	memcpy(*str + idx, proto_ver, proto_len);
	idx += proto_len;
	memcpy(*str + idx, other, other_len);
     
	//
	// if (method) free(method);
     
	if (url) free(url);
	if (proto_ver) free(proto_ver);
	if (other) free(other);
	if (p && !strlen(p)) free(p);
     
	return 1;
}

/**
 * 作用： 分析connect请求字符串。 
 * 参数：conn_str :client发送的请求字符。
 *       str     :发送到WEB SERVER的数据。
 *       host    :从get_str取出的host地址
 *       port    :端口。 
 * 返回值：负数为错误。 
 */
int Http::conn(char* conn_str, char** str, char** host, char** port, char** path) {
	//char *method = "connect";
	char* url = NULL;           //url所有字符        
	char* proto_ver = NULL;     //http协议及版本 
	char* other = NULL;         //其它部分 
	char* p = NULL;    //保存临时位置 
     
	conn_str += 5;
	//找到url部分
	if (strchr(conn_str, ' ') == 0) conn_str++;
	p = strchr(conn_str, ' ');  //寻找空格位置 
	if (!p ) return -1;
	int url_len = p - conn_str;
	url = (char *)malloc(url_len + 1);
	memset(url, '\0', url_len + 1);
	memcpy(url, conn_str, url_len);
	conn_str += url_len;           //get_str向后移动到协议部分。 
	if(analyse_url_str(url, host, port, path) < 0) {
		//if (method) free(method);
		if (url) free(url);
		return -1;
	}
        
	//协议部分
	if (strncmp(conn_str, " ", 1) == 0) conn_str++;
	p = strstr(conn_str, "\r\n");
	if (!p) {
		//if (method) free(method);
		if (url) free(url);
		return -2;
	}
	int proto_len = p - conn_str;
	proto_ver = (char *)malloc(proto_len + 1);
	memset(proto_ver, '\0', proto_len + 1);
	memcpy(proto_ver, conn_str, proto_len);
	conn_str += proto_len;

	int other_len = strlen(conn_str);
	other = (char *)malloc(other_len + 1);
	memset(other, '\0', other_len + 1);
	memcpy(other, conn_str, other_len);
     
	//提交给web服务器的请求字符串。 
	//形式如同：POST / http/1.0
	//          等 
	//     char *str; 
	int str_len = 7 + 1 + strlen(*path) + 1 + proto_len + other_len; 
	*str = (char *) malloc(str_len + 1);
	memset(*str, '\0', str_len + 1);
	int idx = 0;
	memcpy(*str + idx, "POST ", 5);
	idx += 5;
	memcpy(*str + idx, *path, strlen(*path));
	idx += strlen(*path);
	memcpy(*str + idx, " ", 1);
	idx += 1;
	memcpy(*str + idx, proto_ver, proto_len);
	idx += proto_len;
	memcpy(*str + idx, other, other_len);

	//
	// if (method) free(method);
     
	if (url) free(url);
	if (proto_ver) free(proto_ver);
	if (other) free(other);
	if (p && !strlen(p)) free(p);
     
	return 1;
}


/**
 *作用：分析客户端连接。
 *      读取client发送的请求，判断是否为合法的http请求。
 *参数：data     ：client发送的字符. 
 *       str     :发送到WEB SERVER的请求字符。
 *       host    :从get_str取出的host地址
 *       port    :端口。  
 */
 
//不区分大小写的比较
char Http::toi(char c) {
	if(c >= 'a' && c <= 'z') {
		c = c - 'a' + 'A';
	}	
	else if (c >= 'A' && c <= 'Z') {
		c = c -'A' + 'a';
	}
	return c;
}
int Http::strnicmp(const char* s1, const char* s2, size_t n) {
	size_t i = 0;
	for(i = 0; i < n && s1 && s2; i++) {
		if(!(*(s1 + i) == *(s2 + i) || *(s1 + i) == toi(*(s2 + i)))) {
			if(*(s1 + i) > *(s2 + i) || *(s1 + i) > toi(*(s2 + i)))
				return 1;
			else if(*(s1 + i) < *(s2 + i) || *(s1 + i) < toi(*(s2 + i)))
				return -1;	
		}
	}
	return 0;
}

// Http 请求数据分析, 并把结果通过指针带出来
int Http::operator() (char* data, char** str, 
                              char** host, char** port,
                              char** path) 
                            throw (HTTPException, SocketException) {
	try {
            if (strnicmp(data, "get ", 4) == 0) {
		return get(data, str, host, port, path);   
            }
	    else if (strnicmp(data, "POST ", 5) == 0) {
		return post(data, str, host, port, path);
	    }
	    else if (strnicmp(data, "CONNECT ", 8) == 0) {
		printf("connect:%s\r\n",data);
	    	return -1;
	    }
        }
        /*
        catch (HTTPException& ex) {
            write_log(LOGERR, ex.what());
        }
        catch (SocketException& ex) {
            write_log(LOGERR, ex.what()); 
        }*/
        catch (...) {
            throw;
        }
	return -1; // 待处理
}

// 得到错误Http状态码信息
void Http::err_codes(int err_id, char** str) {  
    switch (err_id) {
        default:
            *str = (char *)
                "<html>"
                "<meta http-equiv=\"Content-Type\" content=\"text/html;"
                " charset=utf-8\" />"
                "<title>error</title>"
                "<body>error<r>error!</body></html>\0";
            break;
        case 403:
            *str = (char *)
                "<html>"
                "<meta http-equiv=\"Content-Type\" content=\"text/html;"
                " charset=utf-8\" />"
                "<title>access deny</title>"
                "<body>error:403<br>access deny</body></html>\0";
            break;
        case 404:
            *str = (char *)
                "<html>"
                "<meta http-equiv=\"Content-Type\" content=\"text/html;"
                " charset=utf-8\" />"
                "<title>未找到服务器</title>"
                "<body>error:404<br>未找到服务器</body></html>\0";
            break;
        case 500:
            *str = (char *)
                "<html>"
                "<meta http-equiv=\"Content-Type\" content=\"text/html;"
                " charset=utf-8\" />"
                "<title>server error</title>"
                "<body>error:500<br>server error</body></html>\0";
            break;
    }
}

