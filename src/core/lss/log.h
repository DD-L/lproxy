#ifndef _LSS_LOG_H_1
#define _LSS_LOG_H_1
/*************************************************************************
	> File Name:    log.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/15 11:55:58
 ************************************************************************/
#include <log/init_simple.h>

namespace lproxy {
namespace log {

enum Which { LOCAL = 0, SERVER = 1 };

// function lproxy::log::output_thread
void output_thread(Which which);

} // namespace lproxy::log
} // namespace lproxy


#endif // LSS_LOG_H_1
