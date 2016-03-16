/*************************************************************************
	> File Name:    log.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/15 11:55:58
 ************************************************************************/

#include <lss/log.h>
#include <lss/config.h>
#include <memory>
#include <iostream>
#include <fstream>
#include <functional>

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

    /////////////////////////////////////////////////////////
    // 绑定 输出对象

#ifndef LOGOUTPUT2
    // 日志输出到std::cout;
    // 输出权重 大于等于 TRACE 级别的日志;
    // 日志输出格式采用自定义格式
    logoutput.bind(std::cout, makelevel(TRACE), 
            std::bind(lproxy::log::output_format, std::placeholders::_1));
#else
    // 日志输出到std::cout;
    // 只输出 TRACE DEBUG INFO WARN ERROR FATAL 级别的日志;
    // 日志输出格式采用自定义格式
    logoutput.bind(std::cout, 
            {
                makelevel(TRACE), makelevel(DEBUG),
                makelevel(INFO),  makelevel(WARN),
                makelevel(ERROR), makelevel(FATAL)
            },
            std::bind(lproxy::log::output_format, std::placeholders::_1));
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
        // 只输出权重大于等于 ERROR  级别的日志
        // 日志输出格式采用自定义格式
        logoutput.bind(logfile, makelevel(ERROR),
                std::bind(lproxy::log::output_format, std::placeholders::_1));
#else
        // 只输出 ERROR FATAL 的日志
        // 日志输出格式采用自定义格式
        logoutput.bind(logfile, { makelevel(ERROR), makelevel(FATAL) },
            std::bind(lproxy::log::output_format, std::placeholders::_1));
#endif // if not define(LOGOUTPUT2)
    }

    /////////////////////////////////////////
    // 日志输出

    std::shared_ptr<LogVal> val = std::make_shared<LogVal>();
    while (true) {
        logoutput(val);
        // TODO
        // do something
    } 

    logoutput.unbind(std::cout);

} // function lproxy::log::output_thread


// function lproxy::log::output_format
sdata_t lproxy::log::output_format(const std::shared_ptr<LogVal>& val) {
    std::ostringstream oss;
    oss << log_tools::time2string(val->now)
        << " ["
        << std::right << std::setw(5)
        << val->log_type
        << "] " << val->msg << "\t[pid:"
        << val->pid << "] "
        << lproxy::log::basename(val->file_name) 
        << ":" << val->line_num
        << ' ' << val->func_name
        << val->extra
        << std::endl;
    return oss.str();
} // function lproxy::log::output_format


