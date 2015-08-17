/*************************************************************************
	> File Name: test*.cpp
	> Author: D_L
	> Mail: deel@d-l.top 
	> Created Time: 2015/8/15 8:20:10
 ************************************************************************/

#include <iostream>
#include "log_types.h"
#include "priority_queue.h"
#include "store.h"

using std::string;

/**
 *  日志记录数据自定义扩展
 */ 
#include <sstream>
class LogValExtraExample : public LogVal::Extra {
	public:
		LogValExtraExample(const string& str, const int& i)
				: _str_test1(str), _int_test2(i) {}
		LogValExtraExample(string&& str, const int& i)
				: _str_test1(std::move(str)), _int_test2(i) {}
		// @overwrite
		virtual const string format() {
			std::ostringstream oss;
			oss << " {extra_data:" << _str_test1 << ","	<< _int_test2 << "}"; 
			return  oss.str(); // 可以返回临时对象而不用担心效率问题
		}
	private:
		string  _str_test1;
		int     _int_test2;
}; 


void test() {
	
	typedef log_tools::priority_queue<LogVal, LogType> LogQueue;
	
	// 优先因子元素只有一个: FATAL, 即优先输出FATAL
	LogType factors[1] = {FATAL};
	std::vector<LogType> vfactor(factors, factors + 1);
	LogQueue::settings(&LogVal::log_type, vfactor);

	typedef Store<LogVal, LogQueue> LogStore;
	//typedef Store<LogVal, boost::lockfree::queue<LogVal> > LogStore;
	//typedef Store<LogVal> LogStore;
	
	LogStore& logstore = LogStore::get_mutable_instance();
	logstore.push({ 
			log_tools::local_time(), INFO, "1", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__, 
			std::make_shared<LogValExtraExample>("test1", 100)
	});
	logstore.push({ 
			log_tools::local_time(), WARN, "2", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__, 
			std::make_shared<LogValExtraExample>("test2", 200)
	});
	logstore.push({ 
			log_tools::local_time(), FATAL, "3", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__, 
			std::make_shared<LogValExtraExample>("test3", 300)
	});
	logstore.push({ 
			log_tools::local_time(), DEBUG, "4", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__, 
			std::make_shared<LogValExtraExample>("test4", 400)
	});
	// output
	LogVal val;
	for (int i = 0; i < 4; ++i) {
		logstore.pop(val);
		std::cout << log_tools::time2string(val.now)
			<< " [" << log_tools::logtype2string(val.log_type) 
			<< "] " << val.msg << " [p:" 
			<< val.pid << "] [F:" << val.func_name << "] " 
			<< val.file_name << ":" << val.line_num 
			<< *val.extra  // <-- 附加数据
			<< std::endl;
	}

}

int main() {
	test();
	return 0;
}


