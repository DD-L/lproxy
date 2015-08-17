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
#include <memory> // std::shared_ptr

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


// struct message / log data
class LogVal {
public:
	typedef log_tools::ptime  ptime;
	typedef log_tools::pid_t  pid_t;
	class	Extra; // 附加数据接口类

public:
	ptime                  now;
	LogType                log_type;
	std::string            msg;
	pid_t                  pid;
	std::string            func_name;
	std::string            file_name;
	unsigned int           line_num;
	
	// 附加数据. 添加附加数据推荐在LogVai::Extra的子类中去完成，
	std::shared_ptr<Extra> extra;
	// 弃用void*附加数据字段, 类型极其不安全, 且内存难于管理 
	//void*                extra;  // 附加数据, 弃用

public:
	class Extra {
		public:
			virtual const std::string format(void) = 0;
			virtual ~Extra() {}
			friend std::ostream& operator<< (
					std::ostream&         os, 
					const LogVal::Extra&  e) {
				return os << std::move(const_cast<Extra&>(e).format());
			}
	};
private:
	// 默认附加数据, 并演示自定义附加数据的使用
	class ExtraNone : public LogVal::Extra {
	public:
		virtual const std::string format() {
			return "";
		}
	};

// 缺省构造/拷贝构造/operator=/析构 
public:
	virtual ~LogVal() {};
	LogVal(const LogVal::ptime& time = log_tools::local_time(), 
		const LogType& logtype = WARN, 
		const std::string& message = "",
		const LogVal::pid_t& thread_id = log_tools::get_pid(),
		const std::string& function_name = "UNKNOWN_FUNCTION",
		const std::string& filename = "UNKNOWN_FILENAME",
		const unsigned int& line = 0,
		std::shared_ptr<Extra> extra_data = std::make_shared<ExtraNone>()) :
				now(time), log_type(logtype), msg(message), 
				pid(thread_id),	func_name(function_name), 
				file_name(filename), line_num(line), 
				extra(extra_data) {}
	LogVal(const LogVal& that):
			now(that.now), log_type(that.log_type),
			msg(that.msg), pid(that.pid), func_name(that.func_name),
			file_name(that.file_name), line_num(that.line_num),
			extra(that.extra) {}
	LogVal(LogVal&& that) :
			now(std::move(that.now)), log_type(that.log_type),
			msg(std::move(that.msg)), pid(std::move(that.pid)),
			func_name(std::move(that.func_name)),
			file_name(std::move(that.file_name)), line_num(that.line_num),
			extra(that.extra) {}

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
			extra     = that.extra;
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
			extra     = that.extra;
		}
		return *this;
	}

};


#endif // LOG_TYPES_H
