/********************************
 * 
 * file: log_types.cpp
 *		- log 模块用到的自定义类型
 *
*********************************/


#include "log/log_types.h"

// log types
/** 
 * TRACE  追踪
 * DEBUG  细粒度信息事件
 * INFO   消息在粗粒度级别上突出强调应用程序的运行过程
 * WARN   出现潜在错误的情形。
 * ERROR  虽然发生错误事件，但仍然不影响系统的继续运行
 * FATAL  严重的错误事件将会导致应用程序的退出
 */
/*
*/
/*
// 默认内置 6 种日志级别
MAKE_LOGLEVEL(TRACE,  0); // TRACE 权重为0
MAKE_LOGLEVEL(DEBUG, 10); // DEBUG 权重为10
MAKE_LOGLEVEL(INFO , 20); // INFO  权重为20
MAKE_LOGLEVEL(WARN , 30); // WARN  权重为30
MAKE_LOGLEVEL(ERROR, 40); // ERROR 权重为40
MAKE_LOGLEVEL(FATAL, 50); // FATAL 权重为50
*/

namespace log_tools {
	/*
	typedef boost::posix_time::ptime          ptime;
	typedef boost::posix_time::microsec_clock microsec_clock;
	typedef boost::thread::id                 tid_t;
	*/
	// get the current time
	const ptime local_time() {
		return microsec_clock::local_time();
	}
	// boost::posix_time::ptime to std::string
	const std::string time2string(const ptime& time_point) {
		return boost::posix_time::to_simple_string(time_point);
	}
	// get the current thread id
	const tid_t get_tid() {
		return boost::this_thread::get_id();
	}

	void print_s(std::ostringstream&& oss, 
			std::ostream& os/* = std::cout*/) {
		assert(&os == &std::cout || &os == &std::cerr);
		boost::mutex::scoped_lock lock(log_tools::print_lock());
		os << oss.str();
	}
} // namespace log_tools

