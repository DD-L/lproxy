log/logstore_lockfree.h

----

# LogStore_lockfree

```cpp
class LogStore_lockfree : public LogStoreInterface;
```

### 类摘要

```cpp
class LogStore_lockfree : public LogStoreInterface {
    public:
        virtual void push(LogVal* val) override;
		virtual void pop(std::shared_ptr<LogVal>& val) override;
		LogStore_lockfree(void);
		LogStore_lockfree(const LogStore_lockfree&) = delete;
        LogStore_lockfree& operator=(const LogStore_lockfree&) = delete;
	private:
		typedef Store<LogVal*, boost::lockfree::queue<LogVal*> > __Log_Store;
};
```

注意，这里的 pop 方法在执行时，如果遇到底层容器元素为空的情况，会一直阻塞线程，直到不为空。

目前的实现方式比较简陋, 遗留：

```cpp
virtual void pop(std::shared_ptr<LogVal>& val) override {
	LogVal* value = nullptr;
	// 如果仓库为空, 就阻塞等待, 直到不为空
	while (! m_logstore.pop(value)) {
		// TODO
		std::this_thread::yield();

		std::this_thread::sleep_for(std::chrono::milliseconds(29));
		// sleep 29 / 1000 sec
		//std::this_thread::sleep_for(std::chrono::nanoseconds(50));
        // sleep 50 * 1 / 1000,000,000 sec
        // 初步测试的结果是, [可能结果并不可靠, 遗留 TODO ]
        // 1. 在4核CPU主机 + Cygwin, 即使 sleep 1 纳秒, 
        //      CPU 百分比 也几乎是 0
        // 2. 在2核CPU主机 + linux, 需 sleep 29 milliseconds, 
        //      CPU 占用比较可观 (0.3%上下浮动)
     }
     assert(value);
     val.reset(value);
}
```

