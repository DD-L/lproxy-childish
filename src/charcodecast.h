#ifndef _CHARCODECAST_H
#define _CHARCODECAST_H

#include <string>
#include <iconv.h>
using namespace std;
/**
 *  字符编码转换
 */
class CharCodeCast {
    public: 
        static string code_source;
        static string code_dest;
        static bool isvalid;
    public:
        CharCodeCast(size_t bufsize = 1024);
        ~CharCodeCast();
        const char* operator() (const string& str);
    private:
        iconv_t _cd;
        size_t _bufsize;
        char* _buf;
};
typedef CharCodeCast CCC;

#endif // _CHARCODECAST_H

