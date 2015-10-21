#ifndef _LOGSTORE_PRIORITY_H_1
#define _LOGSTORE_PRIORITY_H_1
/*************************************************************************
	> File Name:    logstore_priority.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/10/17 17:10:29
 ************************************************************************/

#include "log/logstoreinterface.h"
#include "log/priority_queue.h"
#include "store/store.h"

class LogStore_priority : public LogStoreInterface {
public:
	/*
	virtual void push(LogVal&& val) override {
		m_logstore.push(std::move(val));	
	}
	*/
	virtual void push(LogVal* val) override {
		m_logstore.push(val);	
	}
	
	virtual void pop(std::shared_ptr<LogVal>& val) override {
		//assert(val);
		// Deleter: 'do nothing', in order to avoid 'double free'
		//val.reset(&(m_logstore.pop()), [](LogVal*){/*do nothing*/});
		val.reset(m_logstore.pop());
	}
	
	LogStore_priority(void) 
		: m_logstore(__Log_Store::get_mutable_instance()) {
		
			// output FATAL priority
			LogType factors[1] = { makelevel(FATAL) };
			std::vector<LogType> vfactor(factors, factors + 1);

			LogQueue::settings(&LogVal::log_type, vfactor);
	}

public:
	LogStore_priority(const LogStore_priority&) = delete;
	LogStore_priority& operator=(const LogStore_priority&) = delete;

private:
	typedef log_tools::priority_queue<LogVal*, LogType> LogQueue;
	typedef Store<LogVal*, LogQueue>                    __Log_Store;
	__Log_Store&                                        m_logstore;
};


#endif // _LOGSTORE_PRIORITY_H_1
