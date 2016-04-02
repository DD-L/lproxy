#ifndef _MACROS_H_1
#define _MACROS_H_1
/*************************************************************************
	> File Name:    macros.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/8 8:56:23
 ************************************************************************/

// g++ -o test main.cpp -I/path/to/lproxy/contrib/boost/boost_1_57_0 -std=c++1y /path/to/lproxy/src/core/boost_build/link/sysprebuild.o
#ifdef __CYGWIN__ 
// 编译时必须添加宏__USE_W32_SOCKETS. 可能需要将终端编码设置成 GBK
#	include <w32api/_mingw_mac.h> // for __MSABI_LONG
#   ifndef _WIN32_WINDOWS
#	    define _WIN32_WINDOWS // or @win7: -D_WIN32_WINNT=0x0601
#   endif
// g++ -o test main.cpp -I/path/to/lproxy/contrib/boost/boost_1_57_0 -D__USE_W32_SOCKETS -std=c++1y  /path/to/lproxy/src/core/boost_build/link/sysprebuild.o  -lws2_32
#endif


#ifndef BOOST_REGEX_NO_LIB
#define BOOST_REGEX_NO_LIB
#endif

#ifndef BOOST_DATE_TIME_SOURCE
#define BOOST_DATE_TIME_SOURCE
#endif

#ifndef BOOST_SYSTEM_NO_LIB
#define BOOST_SYSTEM_NO_LIB
#endif

#endif // _MACROS_H_1
