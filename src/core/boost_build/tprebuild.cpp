#define BOST_ALL_NO_LIB
#include <boost/thread.hpp>

#ifdef _MSC_VER
namespace boost { void tss_cleanup_implemented(void) {} }
#include <libs/thread/src/win32/thread.cpp>
#include <libs/thread/src/win32/tss_dll.cpp>
#include <libs/thread/src/win32/tss_pe.cpp>
#else
#include <libs/thread/src/pthread/thread.cpp>
#include <libs/thread/src/pthread/once.cpp>
#endif
