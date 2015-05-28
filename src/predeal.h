/**
 *	main 函数参数处理
 */

#ifndef _PREDEAL_H
#define _PREDEAL_H
#include <string>
using namespace std;
class PreDeal {
    private:
        int _argc;
        char** _argv;
        string _err_msg;
        string _app_name;
    public:
        PreDeal(int argc, char *argv[], const string& err_msg = "");
        void appName(const string appname);
        void set_err_msg(const string& err_msg);
        void operator() (int& port, string& IpList, string& code);
    private:
        void print_err();
        int atoi_port(const char* port_s); //转换成正整数  
};
#endif // _PRESEAL_H
