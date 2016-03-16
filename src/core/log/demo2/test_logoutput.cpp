/*************************************************************************
	> File Name: test_logout.cpp
	> Author: D_L
	> Mail: deel@d-l.top
	> Created Time: 2015/10/25 8:17:22
 ************************************************************************/
#include <iostream>
#include <fstream>
#include <iomanip> // for std::setw

#include "test_logoutput.h"
#include "log/init_simple.h"

void logoutput_thread(void) {
	// 如果输出目标被多个日志线程访问，在编译选项加上 -DLOG_USE_LOCK
	auto& logoutput = LogOutput2_t::get_instance();

	// 日志输出到std::cout;
    // 默认只输出 TRACE DEBUG INFO WARN ERROR FATAL 级别的日志
	// 日志输出格式采用默认格式
	logoutput.bind(std::cout);	

	// 日志输出到 /tmp/log.1 ;
	// 只输出日志 TRACE, DEBUG 和 INFO 级别的日志
	// 日志输出格式采用默认格式
	std::ofstream ofs1("/tmp/log.1", std::ofstream::app);	
	assert(ofs1);
	logoutput.bind(ofs1, {
            makelevel(TRACE),
            makelevel(DEBUG), 
            makelevel(INFO) 
            }); 
	// 日志输出到 /tmp/log.2
	// 只输出 ERROR 和 FATAL 级别的日志
	// 日志输出格式采用自定义格式
	std::ofstream ofs2("/tmp/log.2", std::ofstream::app);
	assert(ofs2);
	logoutput.bind(ofs2, { makelevel(ERROR), makelevel(FATAL) },
			[](const std::shared_ptr<LogVal>& val) -> std::string {
				std::ostringstream oss;
				oss << log_tools::time2string(val->now)
					<< " [" 
					<< std::right << std::setw(5)
					<< val->log_type
					<< "] " << val->msg << "\t[pid:" 
					<< val->pid << "] @"
					<< val->file_name << ":" << val->line_num 
					<< ' ' << val->func_name 
					<< val->extra
					<< std::endl;
				return oss.str();
			});

	std::shared_ptr<LogVal> val = std::make_shared<LogVal>();
	while(true) {
		logoutput(val);
	}
	
	// 解绑 std::cout
	//logoutput.unbind(std::cout);
}

