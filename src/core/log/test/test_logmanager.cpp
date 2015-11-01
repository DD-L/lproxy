/*************************************************************************
	> File Name: test_logmananger.cpp
	> Author: D_L
	> Mail: deel@d-l.top
	> Created Time: 2015/10/26 9:54:16
 ************************************************************************/

#include <iostream>
#include "log/logmanager.h"
#include "log/logstore_priority.h"
#include "log/logstore_lockfree.h"
#include "log/logging.h"


typedef LogManager<LogStore_priority> LogStore;
//typedef LogManager<LogStore_lockfree> LogStore;

void push() {
	logtrace(1 << " logtrace");

	logdebug(2 << " logdebug");

	loginfo(3 << " loginfo");

	logwarn(4 << " logwarn");
	
	logerror(5 << " logerror");

	logfatal(6 << " logfatal");
}

// 日志输出躶用LogManager::pop
void pop() {
	auto& logmanager = LogStore::get_instance();
	std::shared_ptr<LogVal> val = std::make_shared<LogVal>();;
	for (int i = 0; i < 6; ++i) {
		logmanager.pop(val);
		std::cout << log_tools::time2string(val->now)
			<< " [" << val->log_type 
			<< "] " << val->msg << " [p:" 
			<< val->pid << "] [F:" << val->func_name << "] " 
			<< val->file_name << ":" << val->line_num 
			<< val->extra  // <-- 附加数据
			<< std::endl;
	}
}

int main() {
	push();
	pop();
	
}

