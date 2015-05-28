// thread.h
#ifndef _THREAD_H
#define _THREAD_H
#include "except.h"
#include "sock.h"

// 线程抽象类
class Thread {
    public:
        void start() throw (ThreadException);
        virtual void run() = 0;
        virtual ~Thread() {}
    private:
        // 线程过程(调用 线程处理函数)
        static void* run(void* arg);
};

/////////////////////////////////////
// ReadClientThread

// 用于读取 client 请求数据的线程 的类
class ReadClientThread : public Thread {
    private:
        // 要处理的 client Sock
        Sock _sock_c;
    public:
        ReadClientThread(Sock sock) throw (ThreadException);
        virtual void run();
};

#endif // _THREAD_H

