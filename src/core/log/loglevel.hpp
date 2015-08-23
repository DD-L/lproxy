#ifndef LOGLEVEL_H_
#define LOGLEVEL_H_
/*************************************************************************
	> File Name: loglevel.h
	> Author: D_L
	> Mail: deel@d-l.top
	> Created Time: 2015/8/19 17:24:45
 ************************************************************************/

/**
 * 此版的loglevel 是loglevel.h.old的替代品.
 * 
 * 它比 loglevel.h.old 要优秀，但也失去了原有的一些乐趣.
 *
 */ 
#define BOOST_ALL_NO_LIB

#include <memory>
#include <string>
#include <assert.h>
#include <iostream>
#include <cstring>
using std::string;
using std::ostream;

class Log_LevelBase {
private:
	const string name;
	const int    rank;
public:
	Log_LevelBase(const char* _name = "NONELEVEL", const int& _rank = -1) :
			name(_name), rank(_rank) {
		assert(_name);
	}

	//Log_LevelBase(const Log_LevelBase&) = delete;
	// 为了可以被放入容器, 拷贝构造不能少
	Log_LevelBase(const Log_LevelBase& that) :
		name(that.name), rank(that.rank) {}
	Log_LevelBase(Log_LevelBase&& that) :
		name(std::move(that.name)), rank(that.rank) {}
	
	const char* get_name() const { 
		return name.c_str(); 
	}
	inline const int get_rank() const { 
		return rank; 
	}
	virtual ~Log_LevelBase() {}

	friend bool operator== (const Log_LevelBase& l, const Log_LevelBase& r) {
		return l.name == r.name;
	}
	friend bool operator!= (const Log_LevelBase& l, const Log_LevelBase& r) {
		return !(l == r);
	}

	friend bool operator> (const Log_LevelBase& l, const Log_LevelBase& r) {
		return l.rank > r.rank;
	}
	friend bool operator< (const Log_LevelBase& l, const Log_LevelBase& r) {
		return l.rank < r.rank;
	}

	friend ostream& operator<< (ostream& os, const Log_LevelBase& ll) {
		return os << ll.name; 
	}
	friend ostream& operator<< (ostream& os, Log_LevelBase&& ll) {
		return os << std::move(ll.name); 
	}

};

#include <map>              // for std::map
#include <cstring>          // for strncmp
#include <limits>           // for std::numeric_limits
#include <sstream>          // for std::ostringstream 
#include <boost/thread.hpp> // for boost::mutex
#include "except/except.h"  // for LogException
class LogLevelManage {
public:
	LogLevelManage(Log_LevelBase&& llb, 
			const char* filename, const int& linenum) {
		LogLevelManage::add(
				std::forward<Log_LevelBase>(llb),
				filename, linenum
		);
	}

	static const Log_LevelBase& get_level(const char* level,
			const char* filename, const int& linenum) {
		assert(level);
		try {
			return LogLevels.at(level); // C++11	
		}
		catch (const std::out_of_range&) {
			std::ostringstream msg;
			msg << "Attempts to reference a non-existent global constant - '"
				<< level << "' [" << filename << ':' << linenum << "]";
			throw LogException(msg.str());
		}

	}
	void clearwarning(void) { 
		// do nothing
		// 仅仅是消除makelevel在局部域中使用时, 编译器产生的警告:
		// variable '...' set but not used [-Wunused-but-set-variable]
	}
private:
	static void add(Log_LevelBase&& llb, 
			const char* filename, const int& linenum) {
		boost::mutex::scoped_lock lock(levels_lock);

		//typedef std::pair<std::string, Log_LevelBase> value_t;
		typedef std::pair<const char*, Log_LevelBase> value_t;
		if (false == 
				LogLevels.insert(
					value_t(llb.get_name(), std::forward<Log_LevelBase>(llb))
				).second) {

			std::ostringstream msg;
			msg << "Attempts to re-define an existing global constant"
				" - '" << llb.get_name() << "' [" 
				<< filename << ":" << linenum << "]";
			throw LogException(msg.str());
		}
	}


	// 慎用, 可能会引发冲突. 暂时不开放此接口
	size_t erase(const char* level) {
		assert(level);
		boost::mutex::scoped_lock lock(levels_lock);
		return LogLevels.erase(level);
	}
private:
	// 因为关联容器包含指针(const char*), 所以需为其定制比较类型
	// 否则, 使用map::at会出错(得到不想要的结果)
	class key_less {
	public:
		bool operator()(const char* const l, const char* const r) {
			/*
			assert(l && r);
			size_t llen = strlen(l) + 1, rlen = strlen(r) + 1;
			size_t len = (llen > rlen) ? llen : rlen;
			return std::strncmp(l, r, len) < 0;
			*/
			return std::strncmp(l, r, 
						(std::numeric_limits<size_t>::max)()
				   ) < 0;
		}
	};
private:
	static boost::mutex                                   levels_lock;
	// 为了能快速查询, 改成map, key-value对
	static std::map<const char*, Log_LevelBase, key_less> LogLevels;
	// 使用 (const char* + key_less) 方案的原因
	// (key 直接使用const char*, 而不使用std::string) 
	//		是因为key 在被放入map容器中时, 
	//		std::string 也是提供的比较器basic_string::operator<,
	//		而const char* + key_less 的方案，要比使用std::string
	//		少一次构造
};
std::map<const char*, Log_LevelBase, LogLevelManage::key_less> 
LogLevelManage::LogLevels;
boost::mutex LogLevelManage::levels_lock;


// LogLevel 工厂函数
#define MAKE_LOGLEVEL(Level, Rank)\
	auto&& __tmp_var_LogLevelManage_##Level \
		=  LogLevelManage(Log_LevelBase(#Level, Rank), __FILE__, __LINE__);

// MAKE_LOGLEVEL 函数(局部)域版本
#define MAKE_LOGLEVEL_INSIDE(Level, Rank)\
	MAKE_LOGLEVEL(Level, Rank)\
	__tmp_var_LogLevelManage_##Level.clearwarning();

// 全局常量Level修饰符
#define makelevel(Level) \
	LogLevelManage::get_level(#Level, __FILE__, __LINE__)


// 伪装成一个 enum, 可构造一个Level全局常量
class LogType {
	public:
		LogType(const Log_LevelBase& loglevel) :
			log_level(std::make_shared<const Log_LevelBase>(loglevel)) {}
		LogType(const LogType& that) : 
			log_level(that.log_level) {}
		const char* get_name() const {
			return log_level->get_name();
		}
		const LogType& operator=(const LogType& that) {
			if (this != &that) {
				log_level = that.log_level;
			}
			return *this;
		}

		friend bool operator== (const LogType& l, const LogType& r) {
			return *(l.log_level) == *(r.log_level);
		}
		friend bool operator!= (const LogType& l, const LogType& r) {
			return !(l == r);
		}
		friend bool operator> (const LogType& l, const LogType& r) {
			return (*(l.log_level) > *(r.log_level));
		}
		friend bool operator< (const LogType& l, const LogType& r) {
			return (*(l.log_level) < *(r.log_level));
		}
		friend ostream& operator<< (ostream& os, const LogType& lt) {
			return os << lt.get_name(); 
		}
	private:
	   std::shared_ptr<const Log_LevelBase> log_level;
};

#endif // LOGLEVEL_H_

