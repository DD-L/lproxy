/*************************************************************************
	> File Name: loglevel.cpp
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

#include "log/loglevel.h"

// class LogType
bool operator== (const LogType& l, const LogType& r) {
	return *(l.log_level) == *(r.log_level);
}
bool operator!= (const LogType& l, const LogType& r) {
	return !(l == r);
}
bool operator> (const LogType& l, const LogType& r) {
	return (*(l.log_level) > *(r.log_level));
}
bool operator< (const LogType& l, const LogType& r) {
	return (*(l.log_level) < *(r.log_level));
}
bool operator>= (const LogType& l, const LogType& r) {
	return (!(l < r));
}
bool operator<= (const LogType& l, const LogType& r) {
	return (!(l > r));
}
std::ostream& operator<< (std::ostream& os, const LogType& lt) {
	return os << lt.get_name(); 
}

// class LogLevelManage
/*
boost::mutex LogLevelManage::levels_lock;
std::map<const char*, Log_LevelBase, LogLevelManage::key_less> 
LogLevelManage::LogLevels;
*/

LogLevelManage::__default_log_level::__default_log_level() {
	// 默认内置 6 种日志级别
#undef ERROR // https://github.com/DD-L/lproxy/issues/159
	MAKE_LOGLEVEL_INSIDE(TRACE,  0); // TRACE 权重为0
	MAKE_LOGLEVEL_INSIDE(DEBUG, 10); // DEBUG 权重为10
	MAKE_LOGLEVEL_INSIDE(INFO , 20); // INFO  权重为20
	MAKE_LOGLEVEL_INSIDE(WARN , 30); // WARN  权重为30
	MAKE_LOGLEVEL_INSIDE(ERROR, 40); // ERROR 权重为40
	MAKE_LOGLEVEL_INSIDE(FATAL, 50); // FATAL 权重为50
}

// class LogLevelManage
LogLevelManage::LogLevelManage(Log_LevelBase&& llb,  
                               const char* filename,
                               const int& linenum) {
	LogLevelManage::add(llb, filename, linenum);
}

const Log_LevelBase& LogLevelManage::get_level(const char* level,
                                               const char* filename,
                                               const int& linenum) {
	static __default_log_level default_log_level;
	assert(level);
	try {
		return get_LogLevels().at(level); // C++11	
	}
	catch (const std::out_of_range&) {
		std::ostringstream msg;
		msg << "Attempts to reference a non-existent global constant - '"
			<< level << "' [" << filename << ':' << linenum << "]";
		throw LogException(msg.str());
	}
}
void LogLevelManage::clearwarning(void) { 
	// do nothing
	// 仅仅是消除makelevel在局部域中使用时, 编译器产生的警告:
	// variable '...' set but not used [-Wunused-but-set-variable]
}
void LogLevelManage::add(Log_LevelBase& llb, 
                        const char* filename,
                        const int& linenum) {
	boost::mutex::scoped_lock lock(LogLevelManage::get_lock());

	typedef std::pair<const char*, Log_LevelBase> value_t;
	const char* level_name = llb.get_name();
	value_t value = value_t(level_name, llb);
	if ( false == get_LogLevels().insert(value).second ) {

		std::ostringstream msg;
		msg << "Attempts to re-define an existing global constant"
			" - '" << llb.get_name() << "' [" 
			<< filename << ":" << linenum << "]";
		throw LogException(msg.str());
	}
}

size_t LogLevelManage::erase(const char* level) {
	assert(level);
	boost::mutex::scoped_lock lock(LogLevelManage::get_lock());
	return get_LogLevels().erase(level);
}


