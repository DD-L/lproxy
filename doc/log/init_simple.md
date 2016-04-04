log/init_simple.h

-----

# 日志库初始化

包含了必要的头文件，以及类型定义，用户在使用时只需要 `#include <log/init_simple.h>`。

使用者只需留意 宏 `LOG_PRIORITY_QUEUE`、`LOG_LOCKFREE_QUEUE`、和 `LOG_USE_LOCK` 即可。


### 摘要

```cpp
// 定义仓库类型
#include "log/logmanager.h"
#include "log/logstore_priority.h"
#include "log/logstore_lockfree.h"

#ifdef LOG_PRIORITY_QUEUE
	typedef LogManager<LogStore_priority> LogStore;
#else 
#	ifdef LOG_LOCKFREE_QUEUE
	typedef LogManager<LogStore_lockfree> LogStore;
#	else
#		error "you must define macro: 'LOG_PRIORITY_QUEUE' or 'LOG_LOCKFREE_QUEUE'"
#	endif // LOCKFREE_QUEUE
#endif // PRIORITY_QUEUE

// logging
#include "log/logging.h"

// log output
#include "log/logoutput.h"
#include "log/logoutput2.h"
// log_tools::UseLock<true> : 
//     日志输出目的地时使用互斥锁, 避免多线程下对同一输出目标的争抢
// log_tools::UseLock<false>: 
//     日志输出目的地时, 除std::cout和std::cerr外, 不使用互斥锁
#ifdef LOG_USE_LOCK
	typedef LogOutput<log_tools::UseLock<true> > LogOutput_t; 
	typedef LogOutput2<log_tools::UseLock<true> > LogOutput2_t; 
#else
	typedef LogOutput<log_tools::UseLock<false> > LogOutput_t; 
	typedef LogOutput2<log_tools::UseLock<false> > LogOutput2_t; 
#endif // LOG_USE_LOCK
// !!!! IMPORTANT notice !!!!
//
// 注意:
//
// 1. LogOutput_t 的 bind 接口, 只能绑定 最小日志级别. 
//    即日志输出时, 只能输出"大于等于"该绑定级别的日志
//
// 2. LogOutput2_t 的 bind 接口, 绑定的是 日志级别集合. 
//    即日志输出时, 只能输出该集合内"存在"的级别的日志 
//
// 3. LogOutput_t 和 LogOutput2_t 被特意设计成不能在同一进程中同时使用. 
//      如果试图在同一进程中同时使用 LogOutput_t 和 LogOutput2_t 的 bind
//      接口, 会在程序运行时，引发 断言失败:
//      assertion "is_unique_subclass_working(this)" failed
//
// !!!! IMPORTANT notice !!!!
```