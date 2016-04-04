log/logoutput.h

-----

# LogOutput

```cpp
template <typename UseLock = log_tools::UseLock<true> >
class LogOutput : public LogOutput_if;
```

日志输出单件类

### 注意 `LogOutput` 和 `LogOutput2` 在同一个进程中只能选择一个。

1. `LogOutput` 的 `bind` 接口, 只能绑定 最小权重日志级别. 即日志输出时, 只能输出"大于等于"该绑定级别的日志.

2. `LogOutput2` 的 `bind` 接口, 绑定的是 日志级别的集合. 即日志输出时, 只能输出该集合内"存在"的级别的日志.

3. `LogOutput` 和 `LogOutput2` 被特意设计成不能在同一进程中同时使用. 如果试图在同一进程中同时使用 `LogOutput` 和 `LogOutput2` 的 `bind` 接口, 会在程序运行时，引发 断言失败: `assertion "is_unique_subclass_working(this)" failed`


### 类摘要

```cpp
template <typename UseLock = log_tools::UseLock<true> >
class LogOutput : public LogOutput_if {
private:
	LogOutput(void)                        = default;
public:
	LogOutput(const LogOutput&)            = delete;
	LogOutput& operator=(const LogOutput&) = delete;
	virtual ~LogOutput();
public:
	// 
	inline static LogOutput& get_instance(void) {
		static LogOutput instance;
		return instance; 
	}
	// bind/insert ostream, ... 
	template <typename Format = default_format>
	bool bind(std::ostream& os, 
			const LogType& min_loglevel = makelevel(TRACE), 
			Format format = LogOutput_if::default_format());

	// unbind/remove ostream. 
	// Just remove the ostream, but not destruct ostream
	bool unbind(std::ostream& os);
	
	// only outputs one log record.
	//     output the oldest log only once from LogStore.
	//     If the LogStore is empty, it will wait until the
	//     LogStore is not empty, and then output the logs.
	void operator() (std::shared_ptr<LogVal>& val);
	void output_once(std::shared_ptr<LogVal>& val) { this->operator() (val); }

	// returns the specified lock pointer
	// if the parameter, os, is not binding, it will return NULL
	boost::mutex* get_lock(const std::ostream& os) const;

	// Capacity
	// bound ostream count
	inline size_t ostream_count(void) const;
	// it is empty if there are no ostreams.
	inline bool empty() const;
};
```