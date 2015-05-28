/**
 *  日志  
 */

#ifndef _LOG_H
#define _LOG_H

//#include <pthread.h>
//#include <list>
#include <iostream>
using namespace std;

// 日志类型
enum LogType { LOGINFO, LOGALERT, LOGERR };

// 日志输出
void write_log(LogType logType, const string& str);

#endif //_LOG_H
