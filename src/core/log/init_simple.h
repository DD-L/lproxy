#ifndef _INIT_SIMPLE_H_1
#define _INIT_SIMPLE_H_1
/*************************************************************************
	> File Name:    init_simple.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/10/17 17:32:39
 ************************************************************************/


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

// logout
#include "log/logoutput.h"
// log_tools::UseLock<true> : 
//     日志输出目的地时使用互斥锁, 避免多线程下对同一输出目标的争抢
// log_tools::UseLock<false>: 
//     日志输出目的地时, 除std::cout和std::cerr外, 不使用互斥锁
#ifdef LOG_USE_LOCK
	typedef LogOutput<log_tools::UseLock<true> > LogOutput_t; 
#else
	typedef LogOutput<log_tools::UseLock<false> > LogOutput_t; 
#endif // LOG_USE_LOCK

#endif // _INIT_SIMPLE_H_1
