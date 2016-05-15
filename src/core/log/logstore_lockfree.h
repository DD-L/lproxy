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
                // TODO
                std::this_thread::yield();

                std::this_thread::sleep_for(std::chrono::milliseconds(70));
                // sleep 70 / 1000 sec
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
        //LogStore_lockfree(void)
        //    : m_logstore(__Log_Store::get_mutable_instance()) {}
        LogStore_lockfree(void)
            //: m_logstore(__store_private_::STORECAPACITY) {}
            : m_logstore(0) {}
        LogStore_lockfree(const LogStore_lockfree&) = delete;
        LogStore_lockfree& operator=(const LogStore_lockfree&) = delete;
    private:
        // https://github.com/DD-L/lproxy/issues/166
        //typedef Store<LogVal*, boost::lockfree::queue<LogVal*> > __Log_Store;
        //__Log_Store&                                             m_logstore;
        typedef boost::lockfree::queue<LogVal*>  __Log_Store;
        __Log_Store                              m_logstore;
};

#endif // _LOGSTORE_LOCKFREE_H_1

