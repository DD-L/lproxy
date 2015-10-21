#include <iostream>
#include "log/init_simple.h"
#include "log/logging.h"
#include "log/logout.h"


class LogValExtraExample : public LogVal::Extra {
	public:
		LogValExtraExample(const string& str, const int& i)
				: _str_test1(str), _int_test2(i) {}
		LogValExtraExample(string&& str, const int& i)
				: _str_test1(std::move(str)), _int_test2(i) {}
		// @overwrite
		virtual const string format(void) const override {
			std::ostringstream oss;
			oss << " {extra_data:" << _str_test1 << ","	<< _int_test2 << "}"; 
			return  oss.str();
		}
	private:
		string  _str_test1;
		int     _int_test2;
}; 

void push() {
	logtrace("logtrace");
	logtraceEx("logtraceEx", std::make_shared<LogValExtraExample>("logtraceEx_", 100));

	logdebug("logdebug");
	logdebugEx("logdebugEx", std::make_shared<LogValExtraExample>("logdebugEx_", 200));

	loginfo("loginfo");
	loginfoEx("loginfoEx", std::make_shared<LogValExtraExample>("loginfoEx_", 300));

	logwarn("logwarn");
	logwarnEx("logwarnEx", std::make_shared<LogValExtraExample>("logwarnEx_", 400));
	
	logerror("logerror");
	logerrorEx("logerrorEx", std::make_shared<LogValExtraExample>("logerrorEx_", 500));

	logfatal("logfatal");
	logfatalEx("logfatalEx", std::make_shared<LogValExtraExample>("logfatalEx_", 600));
}

void pop() {
	auto& logmanager = LogStore::get_instance();
	std::shared_ptr<LogVal> val = std::make_shared<LogVal>();;
	for (int i = 0; i < 12; ++i) {
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

void pop(int) {
	
	typedef log_tools::priority_queue<LogVal, LogType> LogQueue;
	typedef Store<LogVal, LogQueue>                    __Log_Store;
	__Log_Store& m_logstore = __Log_Store::get_mutable_instance();

	LogVal val;
	for (int i = 0; i < 12; ++i) {
		m_logstore.pop(val);
		std::cout << log_tools::time2string(val.now)
			<< " [" << val.log_type 
			<< "] " << val.msg << " [p:" 
			<< val.pid << "] [F:" << val.func_name << "] " 
			<< val.file_name << ":" << val.line_num 
			<< val.extra  // <-- 附加数据
			<< std::endl;
	}
}


int main() {
	push();

	pop();
	return 0;
}
