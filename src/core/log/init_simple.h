#ifndef _INIT_SIMPLE_H_1
#define _INIT_SIMPLE_H_1
/*************************************************************************
	> File Name:    init_simple.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/10/17 17:32:39
 ************************************************************************/


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

#endif // _INIT_SIMPLE_H_1
