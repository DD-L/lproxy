#ifndef _LSS_LOG_H_1
#define _LSS_LOG_H_1
/*************************************************************************
	> File Name:    log.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/15 11:55:58
 ************************************************************************/
#include <log/init_simple.h>
#include <lss/config.h>
#include <memory>
#include <fstream>

namespace lproxy {

namespace log {
enum Which { LOCAL = 0, SERVER = 1 };

void output_thread(Which which) {
    // 如果输出目标被多个日志线程访问，需在编译选项加上 -DLOG_USE_LOCK
    auto& logoutput = LogOutput_t::get_instance();

    // 日志输出到std::cout;
    // 只输出日志权重大于等于TRACE的日志;
    // 日志输出格式采用默认格式
    logoutput.bind(std::cout);

    sdata_t logfilename = "";
    switch (which) {
    case LOCAL:
        logfilename = local::config::get_instance().get_logfilename();
        break;
    case SERVER:
        logfilename = server::config::get_instance().get_logfilename();
        break;
    default:
        logfilename = "";
        break;
    }
    std::ofstream logfile;
    if (logfilename != "") {
        logfile.open(logfilename, std::ofstream::app);
        assert(logfile);
        // 只输出日志权重大于等于ERROR的日志
        // 日志输出格式采用默认格式
        logoutput.bind(logfile, makelevel(ERROR));
    }

    std::shared_ptr<LogVal> val = std::make_shared<LogVal>();
    while (true) {
        logoutput(val);
        // TODO
        // do something
    } 
    logoutput.unbind(std::cout);
}
} // namespace lproxy::log
} // namespace lproxy


#endif // LSS_LOG_H_1
