/**
 * 此版的loglevel还没开发完整就已经被标记为"废弃"
 * 因为，有更高效和更简单的实现方式.
 * 但是作为一种新方法的尝试，代码仍然被保留了下来
 */ 


#ifndef LOGLEVEL_H_
#define LOGLEVEL_H_
/*************************************************************************
	> File Name: loglevel.h
	> Author: D_L
	> Mail: deel@d-l.top
	> Created Time: 2015/8/18 7:21:05
 ************************************************************************/
#include <memory>
#include <string>
#include <assert.h>
#include <iostream>
#include <cstring>
using std::string;
using std::ostream;

class Log_LevelBase {
public:
	//virtual const char* get_name() const = 0;
	//virtual int get_rank() const = 0;
	virtual const char* get_name() const { return "NONELEVEL"; };
	virtual int get_rank() const { return -1; };
	virtual ~Log_LevelBase() {}
	Log_LevelBase() {}

	friend bool operator== (const Log_LevelBase& l, const Log_LevelBase& r) {
		return !::strcmp(l.get_name(), r.get_name());
	}
	friend bool operator!= (const Log_LevelBase& l, const Log_LevelBase& r) {
		return !(l == r);
	}

	friend bool operator> (const Log_LevelBase& l, const Log_LevelBase& r) {
		return l.get_rank() > r.get_rank();
	}
	friend bool operator< (const Log_LevelBase& l, const Log_LevelBase& r) {
		return l.get_rank() < r.get_rank();
	}

	friend ostream& operator<< (ostream& os, const Log_LevelBase& ll) {
		return os << ll.get_name(); 
	}
	friend ostream& operator<< (ostream& os, 
					std::shared_ptr<Log_LevelBase> llp) {
		return os << llp->get_name();
	}
	friend ostream& operator<< (ostream& os, 
					std::shared_ptr<Log_LevelBase const> cllp) {
		return os << cllp->get_name();
	}
};

#include <set>
class LogLevelManage {
public:
	LogLevelManage(std::shared_ptr<Log_LevelBase> llp) {
		LogLevelManage::add(llp);
	}
	static const std::set<std::shared_ptr<Log_LevelBase> >& levels(void) {
		return LogLevels;
	}
	static const Log_LevelBase& get_level(const char* level) {
		return *get_level_ptr(level);
	}
	static const std::shared_ptr<Log_LevelBase> 
	get_level_ptr(const char* level) {
		assert(level);
		for (auto& it : LogLevels) {
			if (! strcmp(it->get_name(), level)) {
				return it;
			}
		}
		return std::make_shared<Log_LevelBase>(log_level_none);
	}
	void clearwarning(void) { 
		// do nothing
		// 仅仅是消除makelevel在局部域中使用时, 编译器产生的警告:
		// variable '...' set but not used [-Wunused-but-set-variable]
	}
private:
	static void add(std::shared_ptr<Log_LevelBase> llp) {
		assert(LogLevels.insert(llp).second);
	}
private:
	static std::set<std::shared_ptr<Log_LevelBase> > LogLevels;
	static Log_LevelBase log_level_none;
};
std::set<std::shared_ptr<Log_LevelBase> > LogLevelManage::LogLevels;
Log_LevelBase LogLevelManage::log_level_none;

// LogLevel 类工厂 宏函数
#define MAKE_LOGLEVEL(Level, Rank)\
	class Log_##Level : public Log_LevelBase {\
	public: \
		Log_##Level(const char* levelname, int logrank) :\
				name(levelname), rank(logrank) {assert(levelname);}\
		virtual const char* get_name() const {\
			return name.c_str();\
		};\
		virtual int get_rank() const { return rank;}\
	private: string name; int rank;\
	}; \
	auto __tmp_var_LogLevelManage_##Level \
		=  LogLevelManage(std::make_shared<Log_##Level>(#Level, Rank));\

// MAKE_LOGLEVEL 函数(局部)域版本
#define MAKE_LOGLEVEL_INSIDE(Level, Rank)\
	MAKE_LOGLEVEL(Level, Rank)\
	__tmp_var_LogLevelManage_##Level.clearwarning();

// LogLevel 对象工厂 宏函数
#define makelevel(Level) LogLevelManage::get_level_ptr(#Level)



// 伪装成一个 enum
class LogType {
	public:
		LogType(const Log_LevelBase& loglevel) :
			log_level(std::make_shared<Log_LevelBase>(loglevel)) {}
		LogType(std::shared_ptr<Log_LevelBase> loglevel) :
			log_level(loglevel) {}
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

	friend bool operator==(const LogType& l, const LogType& r) {
		return *(l.log_level) == *(r.log_level);
	}
	friend bool operator!=(const LogType& l, const LogType& r) {
		return !(l == r);
	}
	friend bool operator>(const LogType& l, const LogType& r) {
		return (*(l.log_level) > *(r.log_level));
	}
	friend bool operator<(const LogType& l, const LogType& r) {
		return (*(l.log_level) < *(r.log_level));
	}
	friend ostream& operator<< (ostream& os, const LogType& lt) {
		return os << lt.log_level; 
	}
	private:
	   std::shared_ptr<Log_LevelBase> log_level;
};

#endif // LOGLEVEL_H_

