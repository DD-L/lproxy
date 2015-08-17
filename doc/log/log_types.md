log/log_types.h

---------------------------

#LogType

定义了日志类型(级别)
<pre>
enum LogType { 
	TRACE = 0,  DEBUG = 10, 
	INFO  = 20,  WARN = 30, 
	ERROR = 40, FATAL = 50
};
</pre>

推荐使用方式:
 
 * TRACE  跟踪
 * DEBUG  细粒度信息事件
 * INFO   消息在粗粒度级别上突出强调应用程序的运行过程
 * WARN   出现潜在错误的情形。
 * ERROR  虽然发生错误事件，但仍然不影响系统的继续运行
 * FATAL  严重的错误事件将会导致应用程序的退出

#LogVal

日志记录，定义了每条日志所包含的内容.
<pre>
// 摘要
class LogVal {
public:
	typedef log_tools::ptime  ptime;
	typedef log_tools::pid_t  pid_t;
	class Extra; // 扩展数据的抽象类

	ptime               now;       // 当前时间
	LogType             log_type;  // 日志类型
	std::string         msg;       // 日志正文
	pid_t               pid;       // 当前线程id
	std::string         func_name; // 所在函数
	std::string         file_name; // 所在文件
	unsigned int        line_num;  // 所在行号
	std::shared_ptr< Extra >     extra; // 备用(扩展)数据

	// 缺省构造/拷贝构造/operator=/析构
	virtual ~LogVal() {}
	LogVal(const LogVal::ptime& time = log_tools::local_time(), 
		const LogType& logtype = WARN, 
		const std::string& message = "",
		const LogVal::pid_t& thread_id = log_tools::get_pid(),
		const std::string& function_name = "UNKNOWN_FUNCTION",
		const std::string& filename = "UNKNOWN_FILENAME",
		const unsigned int& line = 0,
		std::shared_ptr< Extra > extra_data = std::make_shared< ExtraNone >());
	LogVal(const LogVal& that);
	LogVal(LogVal&& that);
	LogVal& operator= (const LogVal& that);
	LogVal& operator= (LogVal&& that);
private:
	class ExtraNone; // Extra抽象类的实现子类，仅被用在LogVal的缺省构造函数中
};
</pre>
如果LogVal字段未能满足需求，扩展的任务尽量交给LogVal::Extra的子类去完成，不推荐直接更改LogVal， 见下面的LogVal::Extra.

#LogVal::Extra

日志数据的扩展字段的抽象类
<pre>
// 接口比较简单
class LogVal::Extra {
public:
	virtual const std::string format(void) = 0;
	virtual ~Extra() {}
	friend std::ostream& operator<< (
			std::ostream&         os, 
			const LogVal::Extra&  e) {
		return os << std::move(const_cast< Extra& >(e).format());
	}
};
</pre>

若LogVal未能满足需求，那么扩展LogVal::Extra即可，而且任何时候都推荐使用：
<pre>
// 扩展样例：
using std::string;
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
			return  oss.str(); 
			// 可以返回临时对象而不用担心效率问题，原因可以查看父类中调用format()位置的代码
		}
	private:
		string  _str_test1;
		int     _int_test2;
};
 
//
// 应用于容器的 push/pop
// push/pop 接受 一个 LogVal 类型的值
//

buff.push({..., // 最后一个参数填写一个std::shared_ptr< LogValExtraExample >对象即可
		std::make_shared< LogValExtraExample >("test1", 100)
});
LogVal val;
buff.pop(val);
std::cout << log_tools::time2string(val.now)
	<< " [" << log_tools::logtype2string(val.log_type) 
	<< "] " << val.msg << " [p:" 
	<< val.pid << "] [F:" << val.func_name << "] " 
	<< val.file_name << ":" << val.line_num 
	<< *val.extra  // <-- 附加数据, 重写了流输出操作符 
	// or 
	// << val.extra->format()
	<< std::endl;
</pre>

如果之前初始化时(push时)没有给定参数extra任何值，则val.extra将被赋予默认值(类型为LogVal::ExtraNone)，对*val.extra流输出值为空字符串""， 即val.extra->format()为""。

附LogVal::ExtraNone的实现：

<pre>
// 仅仅是重写了format()而已
class LogVal::ExtraNone : public LogVal::Extra {
public:
	virtual const std::string format() {
		return "";
	}
};
</pre>

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
