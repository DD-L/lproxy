#ifndef LOG_TYPES_H
#define LOG_TYPES_H

/********************************
 * 
 * file: log_types.h
 *		- log 模块用到的自定义类型
 *
*********************************/


#include <string>
#include <ctime>
//#include <chrono>
//#include "store.h"


// log types
/** 
 * TRACE
 * DEBUG  细粒度信息事件
 * INFO   消息在粗粒度级别上突出强调应用程序的运行过程
 * WARN   出现潜在错误的情形。
 * ERROR  虽然发生错误事件，但仍然不影响系统的继续运行
 * FATAL  严重的错误事件将会导致应用程序的退出
 */

enum LogType { 
	TRACE = 0,  DEBUG = 10, 
	INFO  = 20,  WARN = 30, 
	ERROR = 40, FATAL = 50
};

// struct message
#include <boost/thread.hpp>
#define BOOST_DATE_TIME_SOURCE
//#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//using namespace boost::gregorian;

namespace log_tools {
	typedef boost::posix_time::ptime          ptime;
	typedef boost::posix_time::microsec_clock microsec_clock;
	typedef boost::thread::id                 pid_t;

	// get the current time
	const ptime local_time() {
		return microsec_clock::local_time();
	}
	// boost::posix_time::ptime to std::string
	const std::string time2string(const ptime& time_point) {
		return boost::posix_time::to_simple_string(time_point);
	}
	// get the current thread id
	const pid_t get_pid() {
		return boost::this_thread::get_id();
	}
	// logtype to const char*
	const char* logtype2string(const LogType& logtype) {
		switch (logtype) {
			case TRACE: return "TRACE";
			case DEBUG: return "DEBUG";
			case INFO:  return "INFO ";
			case WARN:  return "WARN ";
			case ERROR: return "ERROR";
			case FATAL: return "FATAL";
			default:    return "UNKNOWN_LOGTYPE";
		
		}
	}
} // namespace log_tools


struct LogVal {
	typedef log_tools::ptime  ptime;
	typedef log_tools::pid_t  pid_t;

	ptime          now;
	LogType        log_type;
	std::string    msg;
	pid_t          pid;
	std::string    func_name;
	std::string    file_name;
	unsigned int   line_num;

	// 弃用附加数据字段，因为void* 不是类型安全的，并且指针类
	// 型的数据内存不易管理，扩展字段的任务完全可以很容易的在 
	// LogVal的子类中去完成。同LogVal一样，子类也必须要支持
	// 完整意义的"缺省构造/拷贝构造/拷贝赋值"语义。
	//void*               extra;  // 附加数据, 弃用

	// just for std::priority_queue
	//virtual bool operator< (const LogVal& that) const {
	//	// 比值策略:
	//	// 优先输出 FATAL, 其他消息类型按照push到队列的先后顺序输出
	//	if (this->log_type != FATAL && that.log_type == FATAL) 
	//		return true;
	//	else 
	//		return this->now > that.now;
	//	
	//	// 比值策略:
	//	// log_type 最大值优先
	//	//return this->log_type < that.log_type; 
	//}
	
	virtual ~LogVal() {}
	LogVal(const LogVal::ptime& time = log_tools::local_time(), 
		const LogType& logtype = WARN, 
		const std::string& message = "",
		const LogVal::pid_t& thread_id = log_tools::get_pid(),
		const std::string& function_name = "UNKNOWN_FUNCTION",
		const std::string& filename = "UNKNOWN_FILENAME",
		const unsigned int& line = 0) :
			now(time), log_type(logtype), msg(message), 
			pid(thread_id),	func_name(function_name), 
			file_name(filename), line_num(line) {}
	LogVal(const LogVal& that):
		now(that.now), log_type(that.log_type),
		msg(that.msg), pid(that.pid), func_name(that.func_name),
		file_name(that.file_name), line_num(that.line_num) {}
	LogVal(LogVal&& that) :
		now(std::move(that.now)), log_type(that.log_type),
		msg(std::move(that.msg)), pid(std::move(that.pid)),
		func_name(std::move(that.func_name)),
		file_name(std::move(that.file_name)), line_num(that.line_num) {}

	// 'LogVal& LogVal::operator=(const LogVal&)' is implicitly 
	//  declared as deleted because 'LogVal' declares a move constructor 
	//  or move assignment operator
	LogVal& operator= (const LogVal& that) {
		if (&that != this) {
			now       = that.now;
			log_type  = that.log_type;
			msg       = that.msg;
			pid       = that.pid;
			func_name = that.func_name;
			file_name = that.file_name;
			line_num  = that.line_num;
		}
		return *this;	
	}
	LogVal& operator= (LogVal&& that) {
		if (&that != this) {
			now       = std::move(that.now);
			log_type  = that.log_type;
			msg       = std::move(that.msg);
			pid       = std::move(that.pid);
			func_name = std::move(that.func_name);
			file_name = std::move(that.file_name);
			line_num  = that.line_num;
		}
		return *this;
	}
};

#endif // LOG_TYPES_H
