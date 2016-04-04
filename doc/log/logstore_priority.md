log/logstore_priority.h

----

# LogStore_priority

```cpp
class LogStore_priority : public LogStoreInterface;
```

### 类摘要

```cpp
class LogStore_priority : public LogStoreInterface {
public:
	LogStore_priority(void) {
			// 默认优先输出 FATAL
			// output FATAL priority
			LogType factors[1] = { makelevel(FATAL) };
			std::vector<LogType> vfactor(factors, factors + 1);

			LogQueue::settings(&LogVal::log_type, vfactor);
	}
	LogStore_priority(const LogStore_priority&) = delete;
	LogStore_priority& operator=(const LogStore_priority&) = delete;
	virtual void push(LogVal* val) override;
	virtual void pop(std::shared_ptr<LogVal>& val) override;

private:
	typedef log_tools::priority_queue<LogVal*, LogType> LogQueue;
};
```

注意，这里的 pop 方法在执行时，如果遇到底层容器元素为空的情况，会一直阻塞线程，直到不为空。