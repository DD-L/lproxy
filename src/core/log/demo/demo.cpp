#include <iostream>
#include "log/init_simple.h"

#include "test_logoutput.h" // for logoutput_thread 

// Custom log additional data
class LogValExtraExample : public LogVal::Extra {
	public:
		LogValExtraExample(const std::string& str, const int& i)
				: _str_test1(str), _int_test2(i) {}
		LogValExtraExample(std::string&& str, const int& i)
				: _str_test1(std::move(str)), _int_test2(i) {}
		// @overwrite
		virtual const std::string format(void) const override {
			std::ostringstream oss;
			oss << " {extra_data:" << _str_test1 << ","	<< _int_test2 << "}"; 
			return  oss.str();
		}
	private:
		std::string  _str_test1;
		int          _int_test2;
}; 

// mylevel - demo
MAKE_LOGLEVEL(MyLevel1, 100);
void make_mylevel(void) {
	static bool first_run = true;
	if (first_run) {

		// 同一个日志级别只能被定义一次
		MAKE_LOGLEVEL_INSIDE(MyLevel2, 101);

	}
	first_run = false;
}

// Create some logs
void create_logs() {
	logtrace(1 << " logtrace");
	logtraceEx(1 << " logtraceEx", 
			std::make_shared<LogValExtraExample>("logtraceEx_", 100));

	logdebug(2 << " logdebug");
	logdebugEx(2 << " logdebugEx", 
			std::make_shared<LogValExtraExample>("logdebugEx_", 200));

	loginfo(3 << " loginfo");
	loginfoEx(3 <<" loginfoEx", 
			std::make_shared<LogValExtraExample>("loginfoEx_", 300));

	logwarn(4 << " logwarn");
	logwarnEx(4 << " logwarnEx", 
			std::make_shared<LogValExtraExample>("logwarnEx_", 400));
	
	logerror(5 << " logerror");
	logerrorEx(5 << " logerrorEx", 
			std::make_shared<LogValExtraExample>("logerrorEx_", 500));

	logfatal(6 << " logfatal");
	logfatalEx(6 << " logfatalEx", 
			std::make_shared<LogValExtraExample>("logfatalEx_", 600));

	// mylevel
	make_mylevel();
	logging(makelevel(MyLevel1), "7 test MyLevel1", log_tools::get_pid(),
			__func__, __FILE__, __LINE__);
	logging(makelevel(MyLevel2), "7 test MyLevel2", log_tools::get_pid(),
			__func__, __FILE__, __LINE__, 
			std::make_shared<LogValExtraExample>("test MyLevel2", 700));
}


int main() {
	_print_s("start log demo.\n");
	_print_s("create some logs...\n");
	create_logs();
	
	// 如果输出目标被多个日志线程访问，在编译选项加上 -DLOG_USE_LOCK
	// 即日志输出线程数大于1时, 需启用 -DLOG_USE_LOCK
	const int thread_count = 2;
	_print_s("create " << thread_count << " thread(s) to output the logs\n");
	boost::thread_group group;
	try {
		for (int i = 0; i < thread_count; i++) {
			group.create_thread(boost::bind(&logoutput_thread));
		}
	}
	catch (...) {
		_print_s_err("an error has occurred...");
		throw;
	}

	_print_s("create some logs...\n");
	create_logs();

	group.join_all();
	return 0;
}
