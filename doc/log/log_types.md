log_types.h

#LogType

定义了日志类型(级别)
<pre>
enum LogType { 
	TRACE = 0,  DEBUG = 10, 
	INFO  = 20,  WARN = 30, 
	ERROR = 40, FATAL = 50
};
</pre>

#LogVal

定义了每条日志所包含的内容.
<pre>
// 摘要
struct LogVal {
	typedef log_tools::ptime  ptime;
	typedef log_tools::pid_t  pid_t;

	ptime          now;       // 当前时间
	LogType        log_type;  // 日志类型
	std::string    msg;       // 日志正文
	pid_t          pid;       // 当前线程id
	std::string    func_name; // 所在函数
	std::string    file_name; // 所在文件
	unsigned int   line_num;  // 所在行号

	//
	virtual ~LogVal() {}
	LogVal(const LogVal::ptime& time = log_tools::local_time(), 
		const LogType& logtype = WARN, 
		const std::string& message = "",
		const LogVal::pid_t& thread_id = log_tools::get_pid(),
		const std::string& function_name = "UNKNOWN_FUNCTION",
		const std::string& filename = "UNKNOWN_FILENAME",
		const unsigned int& line = 0);
	LogVal(const LogVal& that);
	LogVal(LogVal&& that);
	LogVal& operator= (const LogVal& that);
	LogVal& operator= (LogVal&& that);
};
</pre>
如果LogVal未能满足需求，扩展的任务尽量交给LogVal的子类，不推荐直接更改LogVal.

#日志处理实用工具集

以下定义都被包含在 namespace log_tools 中。

<pre>
typedef boost::posix_time::ptime          ptime;
typedef boost::posix_time::microsec_clock microsec_clock;
typedef boost::thread::id                 pid_t;

// get the current time
const ptime local_time();

// boost::posix_time::ptime to std::string
const std::string time2string(const ptime& time_point);

// get the current thread id
const pid_t get_pid();

// logtype to const char*
const char* logtype2string(const LogType& logtype);
</pre>