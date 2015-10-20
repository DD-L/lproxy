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

#include "loglevel.h"



// class LogLevelManage

boost::mutex LogLevelManage::levels_lock;
std::map<const char*, Log_LevelBase, LogLevelManage::key_less> 
LogLevelManage::LogLevels;

//
// 下面这种全局的定义方式, 必须出现在
// boost::mutex LogLevelManage::levels_lock;
// 定义之后(运行时)
//

// 默认内置 6 种日志级别
MAKE_LOGLEVEL(TRACE,  0); // TRACE 权重为0
MAKE_LOGLEVEL(DEBUG, 10); // DEBUG 权重为10
MAKE_LOGLEVEL(INFO , 20); // INFO  权重为20
MAKE_LOGLEVEL(WARN , 30); // WARN  权重为30
MAKE_LOGLEVEL(ERROR, 40); // ERROR 权重为40
MAKE_LOGLEVEL(FATAL, 50); // FATAL 权重为50


// class LogLevelManage
LogLevelManage::LogLevelManage(Log_LevelBase&& llb,  
                               const char* filename,
                               const int& linenum) {
	LogLevelManage::add(
			//std::forward<Log_LevelBase>(llb),
			std::move(llb),
			filename, linenum
	);
}

/*static*/ const Log_LevelBase& LogLevelManage::get_level(const char* level,
                                                          const char* filename,
                                                          const int& linenum) {
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
void LogLevelManage::clearwarning(void) { 
	// do nothing
	// 仅仅是消除makelevel在局部域中使用时, 编译器产生的警告:
	// variable '...' set but not used [-Wunused-but-set-variable]
}
/*static*/ void LogLevelManage::add(Log_LevelBase&& llb, 
                                    const char* filename,
                                    const int& linenum) {
	boost::mutex::scoped_lock lock(levels_lock);

	//typedef std::pair<std::string, Log_LevelBase> value_t;
	typedef std::pair<const char*, Log_LevelBase> value_t;
	if ( false == 
			LogLevels.insert(
				//value_t(llb.get_name(), std::forward<Log_LevelBase>(llb))
				value_t(llb.get_name(), std::move(llb))
			).second ) {

		std::ostringstream msg;
		msg << "Attempts to re-define an existing global constant"
			" - '" << llb.get_name() << "' [" 
			<< filename << ":" << linenum << "]";
		throw LogException(msg.str());
	}
}

size_t LogLevelManage::erase(const char* level) {
	assert(level);
	boost::mutex::scoped_lock lock(levels_lock);
	return LogLevels.erase(level);
}


