#ifndef _LOGOUTPUT2_H_1
#define _LOGOUTPUT2_H_1


#include "log/logoutput_if.h"

// 
// class LogOutput2
// 
// log_tools::UseLock<true> : 
//     日志输出目的地时使用互斥锁, 避免多线程下对同一输出目标的争抢
// log_tools::UseLock<false>: 
//     日志输出目的地时, 除std::cout和std::cerr外, 不使用互斥锁
//
template <typename UseLock = log_tools::UseLock<true> >
class LogOutput2 : public LogOutput_if {
public:
    // scoped lock
    class scoped_lock {
    public:
        scoped_lock(const std::ostream& os, boost::mutex& lock)
                : _lock(lock), _os_is_std_outerr(LogOutput2::is_std_outerr(os)) {
            if (_os_is_std_outerr) {
                _lock.lock();
                return;	
            }
            if (LogOutput2::_is_uselock()) {				
                _lock.lock();
                return;
            }
        }
        ~scoped_lock() {
            if (_os_is_std_outerr) {
                _lock.unlock();
                return;
            }
            if (LogOutput2::_is_uselock()) {				
                _lock.unlock();
                return;
            }
        }
    private:
        boost::mutex& _lock;
        bool          _os_is_std_outerr;
    }; // class scoped_lock
private:
	// 
	// std::map[std::ostream] = os_property
	//     key   => std::ostream
	//     value => os_property
	//
	struct os_property {
		boost::mutex*     plock;
        std::set<LogType> loglevel_set;
		Format_t          format;
        /*
		os_property(boost::mutex* plock_ = NULL, 
				const std::set<LogType>& loglevel_set_ = { 
                //    makelevel(TRACE), makelevel(DEBUG),
                //    makelevel(INFO),  makelevel(WARN),
                //    makelevel(ERROR), makelevel(FATAL) 
                    },
				const Format_t& format_ = default_format()) :
				 plock(plock_), loglevel_set(loglevel_set_), format(format_) {} 
        */
		os_property(boost::mutex* plock_ = NULL, 
				std::set<LogType>&& loglevel_set_ = { 
                /*
                    makelevel(TRACE), makelevel(DEBUG),
                    makelevel(INFO),  makelevel(WARN),
                    makelevel(ERROR), makelevel(FATAL) 
                */
                    },
				const Format_t& format_ = default_format()) :
				 plock(plock_), loglevel_set(std::move(loglevel_set_)), format(format_) {} 
		os_property(const os_property& that) :
			plock(that.plock), loglevel_set(that.loglevel_set), 
			format(that.format) {}
	}; // struct os_property

private:
	LogOutput2(void)                        = default;
public:
	LogOutput2(const LogOutput2&)            = delete;
	LogOutput2& operator=(const LogOutput2&) = delete;
	virtual ~LogOutput2() {
		std::for_each(_outstream.begin(), _outstream.end(), 
			[this] (std::pair<std::ostream*, os_property> element) -> void {
				this->release_lock(*element.first, element.second.plock);
			});
	}
public:
	// 
	inline static LogOutput2& get_instance(void) {
		static LogOutput2 instance;
		return instance; 
	}

	// bind/insert ostream, ... 
	template <typename Format = default_format>
	bool bind(std::ostream& os, 
			std::set<LogType>&& loglevel_set = { 
                    makelevel(TRACE), makelevel(DEBUG),
                    makelevel(INFO),  makelevel(WARN),
                    makelevel(ERROR), makelevel(FATAL) 
            }, // default bind TRACE, DEBUG, INFO, WARN, ERROR, FATAL
			Format format = default_format()) {

        // assert only one subclass is working
        assert(is_unique_subclass_working(this));
		
		boost::mutex::scoped_lock lock(_outstreamlock);
		//os => {lock, loglevel_set, format}
		auto ret = _outstream.insert(std::pair<std::ostream*, os_property>
				(&os, os_property(new_lock(os), std::move(loglevel_set), format)));	
		return ret.second; 
	}

	// unbind/remove ostream. 
	// Just remove the ostream, but not destruct ostream
	bool unbind(std::ostream& os) {
		boost::mutex::scoped_lock lock(_outstreamlock);
		boost::mutex* tmp_plock = _outstream[&os].plock;
		auto ret = _outstream.erase(&os);
		release_lock(os, tmp_plock);
		return 1 == ret;
	}

	// only outputs one log record.
	//     output the oldest log only once from LogStore.
	//     If the LogStore is empty, it will wait until the
	//     LogStore is not empty, and then output the logs.
	void operator() (std::shared_ptr<LogVal>& val) {
		LogStore::get_instance().pop(val);
		boost::mutex::scoped_lock lock(_outstreamlock);
		std::for_each(_outstream.begin(), _outstream.end(), 
			[&val] (std::pair<std::ostream*, os_property> element) -> void {
                auto& loglevel_set = element.second.loglevel_set;
                if (loglevel_set.find(val->log_type) != loglevel_set.end()) {
					scoped_lock lock(*element.first, *element.second.plock);
					*element.first << element.second.format(val) << std::flush;
				}
			});
	}

    void output_once(std::shared_ptr<LogVal>& val) {
        this->operator() (val);
    }

	// returns the specified lock pointer
	// if the parameter, os, is not binding, it will return NULL
	boost::mutex* get_lock(const std::ostream& os) const {
		typedef boost::mutex mutex_t;
		mutex_t::scoped_lock lock(const_cast<mutex_t&>(_outstreamlock));
		for (auto it = _outstream.cbegin(); it != _outstream.cend(); ++it) {
			if (it->first == &os) {
				return it->second.plock;			
			}
		}
		return NULL;
	}

	// Capacity
	// bound ostream count
	inline size_t ostream_count(void) const {
		typedef boost::mutex mutex_t;
		mutex_t::scoped_lock lock(const_cast<mutex_t&>(_outstreamlock));
		return _outstream.size();
	}
	// when there are no ostreams, it is empty
	inline bool empty() const {
		typedef boost::mutex mutex_t;
		mutex_t::scoped_lock lock(const_cast<mutex_t&>(_outstreamlock));
		return _outstream.empty();
	} 

private:
	boost::mutex                         _outstreamlock;
	std::map<std::ostream*, os_property> _outstream;
	//static UseLock                            _is_uselock; 
	inline static bool _is_uselock(void) {
		static UseLock is_uselock;
		return is_uselock();
	}
}; // class LogOutput2


#endif // _LOGOUTPUT2_H_1
