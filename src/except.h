/**
 *  异常类
 */
#ifndef _EXCEPT_H
#define _EXCEPT_H
//#include "log.h"
#include <string>
using namespace std;
// 服务器异常
class ServerException : public exception {
    private:
        string m_msg;
    public:
	ServerException (void);
	ServerException (const string& msg);
	~ServerException (void) throw ();
	const char* what (void) const throw ();
};
// 线程异常
class ThreadException :
	public ServerException {
public:
	ThreadException (void);
	ThreadException (const string& msg);
};
// 网络异常
class SocketException :
	public ServerException {
public:
	SocketException (void);
	SocketException (const string& msg);
};
// HTTP 异常
class HTTPException : public ServerException {
public:
        HTTPException(void);
        HTTPException(const string& msg);
};
#endif // _EXCEPT_H
