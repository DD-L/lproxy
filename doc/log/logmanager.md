log/LogManager.h

------------------------------

# LogManager

```cpp
template <typename __LogStore>
class LogManager;
```

利用 `LogStoreInterface`, 用来统一管理日志的缓冲区读写的饿汉单件类。

简单方便的使用方式是 `#include <log/init_simple.h>`，然后编译时有个宏选项：

*  `LOG_PRIORITY_QUEUE` 被定义时将使用 优先队列 作为日志底层容器, 即
	```cpp
	typedef LogManager<LogStore_priority> LogStore;
	```

*  `LOG_LOCKFREE_QUEUE` 被定义时将使用 普通的无锁队列 作为底层容器, 即 

	```cpp
	typedef LogManager<LogStore_lockfree> LogStore;
	```

### 类摘要

```cpp
template <typename __LogStore>
class LogManager {
public:
	static_assert(std::is_base_of<LogStoreInterface, __LogStore>::value, 
			"__LogStore should be derived from LogStoreInterface, "
			"such as LogStore_priority, LogStore_lockfree ...");
	static LogManager& get_instance() {
		return m_log_manager; 
	}
	void push(LogVal* val) {
		m_logstore->push(val);
	}
	void pop(std::shared_ptr<LogVal>& val) {
		m_logstore->pop(val);
	}
	LogManager(const LogManager&) = delete;
	LogManager& operator=(const LogManager&) = delete;
	virtual ~LogManager() {}
private:
	LogManager(void) : m_logstore(std::make_shared<__LogStore>()) {
		assert(m_logstore);
	}
private:
	static LogManager                  m_log_manager;
	std::shared_ptr<LogStoreInterface> m_logstore;
};
template <typename __LogStore>
LogManager<__LogStore> LogManager<__LogStore>::m_log_manager;

```

类型 `__LogStore` 必须继承至 `LogStoreInterface`,  比如 `LogStore_priority` 或 `LogStore_lockfree`, 这决定了你要使用的日志容器是 优先队列容器，还是 普通的无锁队列 容器。


### 示例

```cpp
#include <iostream>
#include "log/logmanager.h"
#include "log/logstore_priority.h"
#include "log/logstore_lockfree.h"
#include "log/logging.h"

typedef LogManager<LogStore_priority> LogStore;
// 或者如下定义
//typedef LogManager<LogStore_lockfree> LogStore;

void push() {
	logtrace(1 << " logtrace");
	logdebug(2 << " logdebug");
	loginfo(3 << " loginfo");
	logwarn(4 << " logwarn");
	logerror(5 << " logerror");
	logfatal(6 << " logfatal");
}

// 日志输出躶用 LogManager::pop
void pop() {
	auto& logmanager = LogStore::get_instance();
	std::shared_ptr<LogVal> val = std::make_shared<LogVal>();;
	for (int i = 0; i < 6; ++i) {
		logmanager.pop(val);
		std::cout << log_tools::time2string(val->now)
			<< " [" << val->log_type 
			<< "] " << val->msg << " [t:" 
			<< val->tid << "] [F:" << val->func_name << "] " 
			<< val->file_name << ":" << val->line_num 
			<< val->extra  // <-- 附加数据
			<< std::endl;
	}
}

int main() {
	push();
	pop();
    return 0;	
}

```

