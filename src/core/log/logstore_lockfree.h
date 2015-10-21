#ifndef _LOGSTORE_LOCKFREE_H_1
#define _LOGSTORE_LOCKFREE_H_1
/*************************************************************************
	> File Name:    logstore_lockfree.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/10/17 17:22:06
 ************************************************************************/

#include "log/logstoreinterface.h"
#include "store/store.h"

class LogStore_lockfree : public LogStoreInterface {
public:
	/*
	virtual void push(LogVal&& val) override {
		m_logstore.push(new LogVal(std::move(val)));
		// 这里new出来的内存会交给后面的pop中的std::share_ptr来管理(释放)
	}
	*/
	virtual void push(LogVal* val) override {
		m_logstore.push(val);
	}
	virtual void pop(std::shared_ptr<LogVal>& val) override {
		//assert(val);
		LogVal* value = nullptr;
		m_logstore.pop(value);
		assert(value);
		val.reset(value);
	}
	LogStore_lockfree(void) 
		: m_logstore(__Log_Store::get_mutable_instance()) {}
	LogStore_lockfree(const LogStore_lockfree&) = delete;
	LogStore_lockfree& operator=(const LogStore_lockfree&) = delete;
private:
	typedef Store<LogVal*, boost::lockfree::queue<LogVal*> > __Log_Store;
	__Log_Store&                                             m_logstore;
};

#endif // _LOGSTORE_LOCKFREE_H_1

