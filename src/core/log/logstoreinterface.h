#ifndef _LOGSTOREINTERFACE_H_1
#define _LOGSTOREINTERFACE_H_1
/*************************************************************************
	> File Name:    logstoreinterface.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/10/17 17:07:04
 ************************************************************************/

#include <assert.h>
#include "log/log_types.h"

// LogStore 抽象类
class LogStoreInterface {
public:
	//virtual void push(LogVal&&) = 0;
	virtual void push(LogVal*) = 0;
	virtual void pop(std::shared_ptr<LogVal>& val) = 0;
/*
public:
	void push(LogVal* val) {
		assert(val);
		push(std::move(*val));
	}
*/
	virtual ~LogStoreInterface() {}
};

#endif //_LOGSTOREINTERFACE_H_1
