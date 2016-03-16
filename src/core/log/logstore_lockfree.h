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
#include <thread>
#include <chrono>

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
            // 如果仓库为空, 就阻塞等待, 直到不为空
            while (! m_logstore.pop(value)) {
                //std::this_thread::yield();

                //std::this_thread::sleep_for(std::chrono::milliseconds(1));
                // sleep 1 / 1000 second
                std::this_thread::sleep_for(std::chrono::nanoseconds(50));
                // sleep 50 * 1 / 1000,000,000 second
                // 初步测试的结果是即使 sleep 1 纳秒, CPU 百分比 也几乎是 0
            }
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

