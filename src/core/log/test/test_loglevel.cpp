/*************************************************************************
	> File Name: test*.cpp
	> Author: D_L
	> Mail: deel@d-l.top 
	> Created Time: 2015/8/19 18:10:10
 ************************************************************************/

#include <iostream>
#include "log/log_types.h"
#include "log/priority_queue.h"
#include "store/store.h"

using std::string;


// 全局域 测试
MAKE_LOGLEVEL(MyLevel1, 100);

void make() {
	// 函数域测试
	MAKE_LOGLEVEL_INSIDE(MyLevel2, 100);
}

void test() {
	make();
	MAKE_LOGLEVEL_INSIDE(MyLevel5, 200);
	MAKE_LOGLEVEL_INSIDE(MyLevel3, 300);
	
	typedef log_tools::priority_queue<LogVal, LogType> LogQueue;
	
	// 优先因子元素有两个: FATAL, 和MyLevel2 
	// 即依次优先输出FATAL, MyLevel
	LogType factors[2] = {makelevel(FATAL), makelevel(MyLevel2)};
	std::vector<LogType> vfactor(factors, factors + 2);
	LogQueue::settings(&LogVal::log_type, vfactor);

	typedef Store<LogVal, LogQueue> LogStore;
	
	LogStore& logstore = LogStore::get_mutable_instance();
	logstore.push({ 
			log_tools::local_time(), makelevel(INFO), "1", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), makelevel(WARN), "2", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), makelevel(FATAL), "3", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), makelevel(MyLevel2), "4", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), makelevel(FATAL), "5", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), makelevel(MyLevel3), "6", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	logstore.push({ 
			log_tools::local_time(), makelevel(MyLevel1), "7", 
			log_tools::get_pid(),
			__func__, __FILE__, __LINE__ 
	});
	// output
	LogVal val;
	for (int i = 0; i < 7; ++i) {
		logstore.pop(val);
		std::cout << log_tools::time2string(val.now)
			<< " [" << val.log_type/*.get_name()*/
			<< "] " << val.msg << " [p:" 
			<< val.pid << "] [F:" << val.func_name << "] " 
			<< val.file_name << ":" << val.line_num 
			<< val.extra  // <-- 附加数据
			<< std::endl;
	}

}

int main() {
	test();
	return 0;
}


