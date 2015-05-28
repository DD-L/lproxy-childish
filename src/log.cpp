#include "log.h"

void write_log(LogType logType, const string& str) {
    string str_out = "";
    // 显示当前时间
    // ...
    switch (logType) {
        case LOGINFO:
            str_out += "INFO: ";
            break;
        case LOGALERT:
            str_out += "LOGALER: ";
            break;
        case LOGERR:
            str_out += "LOGERR: ";
            break;
        default:
            return;
    }
    cout << (str_out += str) << endl;
}
