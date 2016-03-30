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
#include "log/loglevel.h"

// log types
/** 
 * TRACE  追踪
 * DEBUG  细粒度信息事件
 * INFO   消息在粗粒度级别上突出强调应用程序的运行过程
 * WARN   出现潜在错误的情形。
 * ERROR  虽然发生错误事件，但仍然不影响系统的继续运行
 * FATAL  严重的错误事件将会导致应用程序的退出
 */

// 默认内置 6 种日志级别
//MAKE_LOGLEVEL(TRACE,  0); // TRACE 权重为0
//MAKE_LOGLEVEL(DEBUG, 10); // DEBUG 权重为10
//MAKE_LOGLEVEL(INFO , 20); // INFO  权重为20
//MAKE_LOGLEVEL(WARN , 30); // WARN  权重为30
//MAKE_LOGLEVEL(ERROR, 40); // ERROR 权重为40
//MAKE_LOGLEVEL(FATAL, 50); // FATAL 权重为50


// 1. _print_s(msg)
// 2. _print_s_err(msg)
//  demo:
//		_print_s("123" << 345 << 5.0 << std::endl);
//
// Thread-safe print, to std::cout
#define _print_s(msg)\
	do {\
		std::ostringstream oss;\
		oss << msg;\
		log_tools::print_s(std::move(oss));\
	} while(0) 
// Thread-safe print, to std::cerr
#define _print_s_err(msg)\
	do {\
		std::ostringstream oss;\
		oss << msg;\
		log_tools::print_s(std::move(oss), std::cerr);\
	} while(0) 


#include <boost/thread.hpp>
#ifndef BOOST_DATE_TIME_SOURCE
#define BOOST_DATE_TIME_SOURCE
#endif
//#include <boost/date_time/gregorian/gregorian.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
//using namespace boost::gregorian;

namespace log_tools {
	typedef boost::posix_time::ptime          ptime;
	typedef boost::posix_time::microsec_clock microsec_clock;
	typedef boost::thread::id                 tid_t;

	// get the current time
	const ptime local_time(void);
	// boost::posix_time::ptime to std::string
	const std::string time2string(const ptime& time_point);
	// get the current thread id
	const tid_t get_tid(void);

	// 
	// global print lock
	inline boost::mutex& print_lock(void) {
		static boost::mutex __print_lock;
		return __print_lock;
	}
	void print_s(std::ostringstream&& oss, std::ostream& os = std::cout);
} // namespace log_tools


// struct message / log data
class LogVal {
public:
	typedef log_tools::ptime  ptime;
	typedef log_tools::tid_t  tid_t;
	class	Extra;     // 附加数据接口类
	class   ExtraNone; // Extra抽象类的实现子类

public:
	ptime                  now;
	LogType                log_type;
	std::string            msg;
	tid_t                  tid;
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
			virtual const std::string format(void) const = 0;
			virtual ~Extra() {}
			friend std::ostream& operator<< (
					std::ostream&         os, 
					LogVal::Extra&&       e) {
				return os << std::move(e.format());
			}
			friend std::ostream& operator<< (
					std::ostream&                  os, 
					std::shared_ptr<LogVal::Extra> ep) {
				return os << (ep->format());
			}
	};
	// 默认附加数据, 并演示自定义附加数据的使用
	class ExtraNone : public LogVal::Extra {
	public:
		virtual const std::string format(void) const override {
			return "";
		}
	};

// 缺省构造/拷贝构造/operator=/析构 
public:
	virtual ~LogVal() {};
	LogVal(const LogVal::ptime& time = log_tools::local_time(), 
		const LogType& logtype = makelevel(WARN), 
		const std::string& message = "",
		const LogVal::tid_t& thread_id = log_tools::get_tid(),
		const std::string& function_name = "UNKNOWN_FUNCTION",
		const std::string& filename = "UNKNOWN_FILENAME",
		const unsigned int& line = 0,
		std::shared_ptr<Extra> extra_data = std::make_shared<ExtraNone>()) :
				now(time), log_type(logtype), msg(message), 
				tid(thread_id),	func_name(function_name), 
				file_name(filename), line_num(line), 
				extra(extra_data) {}
	LogVal(const LogVal& that):
			now(that.now), log_type(that.log_type),
			msg(that.msg), tid(that.tid), func_name(that.func_name),
			file_name(that.file_name), line_num(that.line_num),
			extra(that.extra) {}
	LogVal(LogVal&& that) :
			now(std::move(that.now)), log_type(that.log_type),
			msg(std::move(that.msg)), tid(std::move(that.tid)),
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
			tid       = that.tid;
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
			tid       = std::move(that.tid);
			func_name = std::move(that.func_name);
			file_name = std::move(that.file_name);
			line_num  = that.line_num;
			extra     = that.extra;
		}
		return *this;
	}

};


#endif // LOG_TYPES_H
