log/logoutput_if.h

----


# LogOutput_if

```cpp
class LogOutput_if
```

日志输出基类，用户只需关心默认的输出格式。

### 类摘要

```cpp
class LogOutput_if {
protected:
	// 判断进程实例中是只有一个 LogOutput_if 子类对象
	static bool is_unique_subclass_working(LogOutput_if* subclass_object);
protected:
	// 默认的输出格式
	class default_format {
	public:
		const std::string operator() (const std::shared_ptr<LogVal>& val) {
			std::ostringstream oss;
			oss << log_tools::time2string(val->now)
				<< " [" 
				<< std::right << std::setw(5)
				<< val->log_type
				<< "] " << val->msg << "\t[tid:" 
				<< val->tid << "] "
				<< val->file_name << ":" << val->line_num 
				<< ' ' << val->func_name 
				<< val->extra
				<< std::endl;
			return oss.str();
		}
	}; // class default_format

	// 日志输出格式化函数对象
	typedef std::function<std::string(const std::shared_ptr<LogVal>&)> Format_t;
};
```

# log_tools::UseLock

```cpp
template<bool isUseLock>
struct UseLock;
```
被用来实例化日志输出模板类, 只作用于`文件输出`（对 `std::cout` 和 `std::cerr` 无效）。如果日志输出线程是 1 个，则需要`log_tools::UseLock<false>`; 如果日志线程 大于1 个，则需要对输出文件进行加互斥锁保护，`log_tools::UseLock<true>`. 默认情况下，被设计成 日志输出线程是 1 个。

更便捷的使用方式是 `#include <log/init_simple.h>`, 然后在编译时留意 宏开关 `LOG_USE_LOCK` 即可：

在 `log/init_simple.h` 有如下定义：
```cpp
// log_tools::UseLock<true> : 
//     日志输出目的地时使用互斥锁, 避免多线程下对同一输出目标的争抢
// log_tools::UseLock<false>: 
//     日志输出到目的地时, 除std::cout和std::cerr外, 不使用互斥锁
#ifdef LOG_USE_LOCK
	typedef LogOutput<log_tools::UseLock<true> > LogOutput_t; 
	typedef LogOutput2<log_tools::UseLock<true> > LogOutput2_t; 
#else
	typedef LogOutput<log_tools::UseLock<false> > LogOutput_t; 
	typedef LogOutput2<log_tools::UseLock<false> > LogOutput2_t; 
#endif // LOG_USE_LOCK
```

### 类摘要
```cpp
namespace log_tools {
// for class LogOutput
template<bool isUseLock>
struct UseLock {
	constexpr bool operator() (void) const {
		return isUseLock;
	}
};
} //  namespace log_tools
```