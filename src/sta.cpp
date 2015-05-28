/*
 * 为了解决proxy意外退出， 强行重启服务器
 * 不推荐使用此方式启动 proxy
 **/
#include <string>
#include <cstdlib>
#include <unistd.h>
using namespace std;

int main(int argc, char* argv[]) {
    string cmd = "./proxy";
    for (int i = 1; i < argc; ++i) {
        cmd += string(" ") + argv[i];
    }
    while (true) {
        system(cmd.c_str());
        usleep(350000);
    }
    return 0;
}
