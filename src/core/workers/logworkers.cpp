#include "workers/logworkers.h"
#include "log/logoutput.h" // is used in the function process() 
#include <sstream>
#include <ctime>
using namespace std;

/* 处理中断时使用的输出流, 初始值为std::cout */
/*static*/ostream& LogWorkers::m_interrupt_os = cout; 
/* 处理日志转储所使用的Logrotate对象, 初始值为NULL */
/*static*/Logrotate* LogWorkers::logrt = NULL;

// 重写三个虚函数
void LogWorkers::prepare() {
	loginfo("Now start a Workers thread(id:" 
			<< boost::this_thread::get_id() 
			<< ", module:LogWorkers)!");
    this->val = std::make_shared<LogVal>();
}


void LogWorkers::process() {
	// 因为本身是从日志仓库里输出日志, 所以这里不能再往日志仓库扔东西
    LogOutput_t::get_instance().output_once(this->val);

	// 尝试日志转储
	if (logrt) {
		logrt->action();
	}
}

void LogWorkers::interruption_respond() {
	// 因为本身是从日志仓库里输出日志, 所以这里不能再往日志仓库扔东西
	time_t timer = time(NULL);
	string cur_time = asctime(localtime(&timer));
	cur_time.pop_back();
	ostringstream oss;
	oss << cur_time << " [WARNING]: Workers thread(id:" 
		<< boost::this_thread::get_id() << ", module:LogWorkers) is interrupted!" 
		<< " [" << __FILE__ << ':' << __LINE__ << ']' << std::endl;

	// 安全输出到m_interrupt_os
	auto plock = LogOutput_t::get_instance().get_lock(m_interrupt_os);
	if (plock) {
		LogOutput_t::scoped_lock lock(m_interrupt_os, *plock);
		m_interrupt_os << oss.str();
	}
	else {
		m_interrupt_os << oss.str();
	}
}
