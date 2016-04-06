#ifndef _LOGOUTPUT_H_1
#define _LOGOUTPUT_H_1


#include "log/logoutput_if.h"

// 
// class LogOutput
// 
// log_tools::UseLock<true> : 
//     日志输出目的地时使用互斥锁, 避免多线程下对同一输出目标的争抢
// log_tools::UseLock<false>: 
//     日志输出目的地时, 除std::cout和std::cerr外, 不使用互斥锁
//
template <typename UseLock = log_tools::UseLock<true> >
class LogOutput : public LogOutput_if {
public:
    // scoped lock
    class scoped_lock {
    public:
        scoped_lock(const std::ostream& os, boost::mutex& lock)
                : _lock(lock), _os_is_std_outerr(LogOutput::is_std_outerr(os)) {
            if (_os_is_std_outerr) {
                _lock.lock();
                return;	
            }
            if (LogOutput::_is_uselock()) {				
                _lock.lock();
                return;
            }
        }
        ~scoped_lock() {
            if (_os_is_std_outerr) {
                _lock.unlock();
                return;
            }
            if (LogOutput::_is_uselock()) {				
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
		boost::mutex* plock;
		LogType       min_loglevel;
		Format_t      format;
		os_property(boost::mutex* plock_ = NULL, 
				const LogType& min_loglevel_ = makelevel(TRACE),
				const Format_t& format_ = default_format()) :
				 plock(plock_), min_loglevel(min_loglevel_), format(format_) {} 
		os_property(const os_property& that) :
			plock(that.plock), min_loglevel(that.min_loglevel), 
			format(that.format) {}
	}; // struct os_property

	
private:
	LogOutput(void)                        = default;
public:
	LogOutput(const LogOutput&)            = delete;
	LogOutput& operator=(const LogOutput&) = delete;
	virtual ~LogOutput() {
		std::for_each(_outstream.begin(), _outstream.end(), 
			[this] (std::pair<std::ostream*, os_property> element) -> void {
				this->release_lock(*element.first, element.second.plock);
			});
	}
public:
	// 
	inline static LogOutput& get_instance(void) {
		static LogOutput instance;
		return instance; 
	}

	// bind/insert ostream, ... 
	template <typename Format = default_format>
	bool bind(std::ostream& os, 
			const LogType& min_loglevel = makelevel(TRACE), 
			Format format = default_format()) {

        // assert only one subclass is working
        assert(is_unique_subclass_working(this));
		
        boost::mutex::scoped_lock lock(_outstreamlock);
		//os => {lock, min_loglevel, format}
		auto ret = _outstream.insert(std::pair<std::ostream*, os_property>
				(&os, os_property(new_lock(os), min_loglevel, format)));	
		return ret.second; 
	}
	/*
	bool bind(std::ostream& os) {
		return bind(os, makelevel(TRACE), default_format());
	}
	bool bind(std::ostream& os, const LogType& min_loglevel) {
		return bind(os, min_loglevel, default_format());
	}
	template <typename Format>
	bool bind(std::ostream& os, const LogType& min_loglevel, Format format) {
		boost::mutex::scoped_lock lock(_outstreamlock);
		//os => {lock, min_loglevel, format}
		auto ret = _outstream.insert(std::pair<std::ostream*, os_property>
				(&os, os_property(new_lock(os), min_loglevel, format)));	
		return ret.second; 
	}
	*/

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
				if (val->log_type >= element.second.min_loglevel) {
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
	// it is empty if there are no ostreams.
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
}; // class LogOutput


#endif // _LOGOUTPUT_H_1
