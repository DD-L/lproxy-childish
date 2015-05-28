// predeal.cpp

#include <iostream>

#include <cstdlib>
#include <map>
#include "predeal.h"
using namespace std;

PreDeal::PreDeal(int argc, char *argv[], const string& err_msg) 
            : _argc(argc), _argv(argv), _err_msg(err_msg) {}

void PreDeal::appName(const string appname) {
    _app_name = appname;
}
void PreDeal::set_err_msg(const string& err_msg) {
    _err_msg = err_msg;
}

void PreDeal::operator() (int& port, string& IpList, string& code) {
    if (_app_name != string(_argv[0])
    	 &&  ("./" + _app_name) != string(_argv[0]))
            print_err();    	
    if (_argc == 1) {
        return;
    }
    if (_argc == 2) {
        int port_arg = atoi_port(_argv[1]);
        if (port_arg) {
            port = port_arg;
            IpList == "";
        }
        else { 
            print_err();
        }
    }
    else if (_argc > 2 && _argc <= 7 && _argc != 4 && _argc != 6) {
        map <string, string> param;
        typedef map<string, string>::iterator IT;
        param["-p"] = "";
        param["-l"] = "";
        param["-c"] = "";
        for (int pos = 1; pos < _argc; pos += 2) {
            for (IT it = param.begin(); it != param.end(); ++it) {
                if (it->first == string(_argv[pos])) {
                    it->second = _argv[pos + 1];
                    break;
                }
            }
        }

        port = atoi_port(param["-p"].c_str());
        if (!port) print_err();
        IpList = param["-l"];
        code = param["-c"];
    }
    else {
        print_err();
    }
}
void PreDeal::print_err() {
    if (_err_msg != "") {
        cout << _err_msg << endl;
    }
    else {
        cout << 
      "\n\t\b\bproxy port"
      "\n\t\b\bproxy -p port [-l clientIpList] [-c code]\n\n"
      "-p[必选]\tport        : 服务器端(本机)使用的端口.\n"
      "-l[可选]\tclientIpList: 允许客户端的ip列表,多个ip用','隔开;"
      "\n\t\t不指定列表表示允许所有ip.\n"
      "-c[可选]\tcode        : proxy显示的编码,默认为 UTF-8.\n\n";
    }
    exit(-1);
}
int PreDeal::atoi_port(const char* port_s) { //转换成正整数
    int i = 0, port = 0;
    char c = 0;
    for(i = 0; (c = *(port_s + i)); i++) {
        if(c >= '0' && c <= '9') {
            port = port * 10 + c - '0';
        }
        else {
            return 0;
        }
    }
    return port;
}    
