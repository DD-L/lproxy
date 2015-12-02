#ifndef _LOGMANAGER_H_1
#define _LOGMANAGER_H_1
/*************************************************************************
	> File Name:    LogManager.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/10/17 16:59:09
 ************************************************************************/

#include <type_traits> // for std::is_base_of
#include "log/logstoreinterface.h"

template <typename __LogStore>
class LogManager {
public:
	static_assert(std::is_base_of<LogStoreInterface, __LogStore>::value, 
			"__LogStore should be derived from LogStoreInterface, "
			"such as LogStore_priority, LogStore_lockfree ...");
	static LogManager& get_instance() {
		return m_log_manager; 
	}
	//void push(const LogVal& val) {
	//	m_logstore->push(val);
	//}
	/*
	void push(LogVal&& val) {
		m_logstore->push(std::move(val));
	}
	*/
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

#endif //_LOGMANAGER_H_1

