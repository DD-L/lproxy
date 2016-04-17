#ifndef BOOST_ALL_NO_LIB
#define BOOST_ALL_NO_LIB
#endif
#include <boost/thread.hpp>

//#ifdef _MSC_VER
#if defined(_MSC_VER) || defined(__MINGW32__)
namespace boost { void tss_cleanup_implemented(void) {} }
#include <libs/thread/src/win32/thread.cpp>
#include <libs/thread/src/win32/tss_dll.cpp>
#include <libs/thread/src/win32/tss_pe.cpp>
#else
#include <libs/thread/src/pthread/thread.cpp>
#include <libs/thread/src/pthread/once.cpp>
#endif
