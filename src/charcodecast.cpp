#include <string>
#include <cstring>
#include <iconv.h>
#include <cstdio>
#include <iostream>
#include "charcodecast.h"
using namespace std;

/**
 *  字符编码转换
 */

CharCodeCast::CharCodeCast(size_t bufsize) 
    : _cd((iconv_t)-1), _bufsize(bufsize), _buf(NULL) {
    if (code_source != "" && code_dest != "") {
	    _cd = iconv_open(code_dest.c_str(), 
	    		    code_source.c_str());
	    if (_cd == (iconv_t)-1) {
	        perror("CharCodeCast::CharCodeCast, iconv_open");
	        return;
	    }
	    try {
		 _buf = new char[_bufsize];
		 memset(_buf, 0, _bufsize);
	    }
	    catch (exception& ex) {
	    	isvalid = false; // 使其不可用
	    	_buf = NULL;
	    	if (_cd != (iconv_t)-1) { 
		    iconv_close(_cd);
		    _cd = (iconv_t)-1;	
		}
	    	cout << ex.what() << endl;
	    }
    }
}
CharCodeCast::~CharCodeCast() {
    if (_cd != (iconv_t)-1) {
        iconv_close(_cd);
        _cd = (iconv_t)-1;
    }
    if (_buf) {
        delete[] _buf;
        _buf = NULL;
    }
}     

const char* CharCodeCast::operator() (const string& str) {
    if (!isvalid) return str.c_str();
    char* str_in = const_cast<char *>(str.c_str());
    char* buff = _buf;
    if (str_in && _cd != (iconv_t)-1) {
    	size_t len = strlen(str_in);
        int n = iconv(_cd, 
                &str_in, &len, &buff, &_bufsize);
        if (n == -1) {
            perror("CharCodeCast::operator(), iconv");
            return str.c_str();
        }
        return _buf;
    }
    return str.c_str();
}

string CharCodeCast::code_source = "UTF-8";
string CharCodeCast::code_dest = "GB2312";
bool CharCodeCast::isvalid = false;
CCC ccc;






