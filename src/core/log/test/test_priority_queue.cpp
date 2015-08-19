/*************************************************************************
	> File Name: test*.cpp
	> Author: D_L
	> Mail: deel@d-l.top 
	> Created Time: 2015/8/8 8:23:09
 ************************************************************************/

#include <iostream>
#include "log_types.h"
#include "priority_queue.h"
#include "store.h"

using namespace std;
void test() {
	
	typedef log_tools::priority_queue<LogVal, LogType> LogQueue;
	// 1 优先因子为空, 输出顺序同输入的顺序，FIFO
	//std::vector<LogType> vfactor;
	////assert(vfactor.empty());
	
	// 2 优先因子元素只有一个: FATAL, 即优先输出FATAL
	LogType factors[1] = {FATAL};
	std::vector<LogType> vfactor(factors, factors + 1);

	// 3 优先因子元素有2个: FATAL 和 ERROR, 
	// 且 FATAL的优先级大于ERROR, 即优先输出FATAL，然后再优先输出ERROR
	//LogType factors[2] = {FATAL, ERROR};
	//std::vector<LogType> vfactor(factors, factors + 2);
	
	LogQueue::settings(&LogVal::log_type, vfactor);

	typedef Store<LogVal, LogQueue> LogStore;
	//typedef Store<LogVal, std::queue<LogVal> > LogStore;
	
	LogStore& logstore = LogStore::get_mutable_instance();
	logstore.push({ 
			log_tools::local_time(), INFO, "1", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({
			log_tools::local_time(), ERROR, "2", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), FATAL, "3", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), WARN, "4", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), ERROR, "5", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	
	// output
	LogVal val;
	for (int i = 0; i < 5; ++i) {
		logstore.pop(val);
		std::cout << log_tools::time2string(val.now)
			<< " [" << log_tools::logtype2string(val.log_type) 
			<< "] " << val.msg << " [p:" 
			<< val.pid << "] [F:" << val.func_name << "] " 
			<< val.file_name << ":" << val.line_num 
			// << val.extra // default "", 可以省略
			<< std::endl;
	}

}

int main() {
	test();
	return 0;
}


