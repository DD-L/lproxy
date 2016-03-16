/*************************************************************************
	> File Name:    log.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/15 11:55:58
 ************************************************************************/

#include <lss/log.h>
#include <lss/config.h>
#include <memory>
#include <fstream>

using namespace lproxy;

// 日志输出线程函数

// function lproxy::log::output_thread
void lproxy::log::output_thread(lproxy::log::Which which) {
    // 如果输出目标被多个日志线程访问，需在编译选项加上 -DLOG_USE_LOCK
#ifndef LOGOUTPUT2
    auto& logoutput = LogOutput_t::get_instance();
#else
    auto& logoutput = LogOutput2_t::get_instance();
#endif // if not define(LOGOUTPUT2)

#ifndef LOGOUTPUT2
    // 日志输出到std::cout;
    // 默认输出权重 大于等于 TRACE 级别的日志;
    // 日志输出格式采用默认格式
    logoutput.bind(std::cout);
#else
    // 日志输出到std::cout;
    // 默认只输出 TRACE DEBUG INFO WARN ERROR FATAL 级别的日志;
    // 日志输出格式采用默认格式
    logoutput.bind(std::cout);
#endif // if not define(LOGOUTPUT2)

    sdata_t logfilename = "";
    switch (which) {
    case lproxy::log::LOCAL:
        logfilename = local::config::get_instance().get_logfilename();
        break;
    case lproxy::log::SERVER:
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
#ifndef LOGOUTPUT2
        // 只输出 ERROR FATAL 的日志
        // 日志输出格式采用默认格式
        logoutput.bind(logfile, makelevel(ERROR));
#else
        // 只输出权重大于等于 ERROR  级别的日志
        // 日志输出格式采用默认格式
        logoutput.bind(logfile, { makelevel(ERROR), makelevel(FATAL) });
#endif // if not define(LOGOUTPUT2)
    }

    std::shared_ptr<LogVal> val = std::make_shared<LogVal>();
    while (true) {
        logoutput(val);
        // TODO
        // do something
    } 

    logoutput.unbind(std::cout);

} // function lproxy::log::output_thread

