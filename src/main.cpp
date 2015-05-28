#include <cstdlib>
#include "log.h"
#include "server.h"
#include "predeal.h"
#include "signal.h"
#include "charcodecast.h"
#include "http.h"
extern CCC ccc;
// 加入信号支持， 以便结束程序

Server* pSvr= NULL;

static void signal_proc(const int s_no) {
    if (pSvr) {
        pSvr->stop();
        delete pSvr;
        pSvr = NULL;
    }
    write_log(LOGALERT, "");
    write_log(LOGALERT, ccc("强制退出 proxy!"));
    exit(0);
}


int main(int argc, char* argv[]) {
    // 注册信号控制函数
    signal(SIGINT, signal_proc);
    signal(SIGQUIT, signal_proc);
    
    // 参数处理
    int port = 2222;
    string clientIpList = "";
    string code = "";
    PreDeal pre(argc, argv);
    pre.appName("proxy");
    pre(port, clientIpList, code);
    
    // 编码转换
    if (!Http::strnicmp(code.c_str(), "GBK", 3) 
    	|| !Http::strnicmp(code.c_str(), "GB2312", 6)) {
    	CCC::code_source = "UTF-8";
    	CCC::code_dest = "GB2312";    
    	CCC::isvalid = true; // 使其可用
    }
#ifdef _DEBUG
    cout << "port = " << port << endl;
    cout << "clientIpList = " 
         << ((clientIpList == "") ? "(nul)" : clientIpList) << endl;
    cout << "code = "
    	 << ((code == "") ? "(nul)" : code) << endl; 

#endif
    // 启动服务器
    write_log(LOGINFO, "proxy server start up!");
    Server* pServer = new Server(port, clientIpList);
    pSvr = pServer;
    pServer->start(); // 阻塞函数
    write_log(LOGINFO, "proxy server shut down!");
    return 0;
}
