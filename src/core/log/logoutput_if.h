#ifndef _LOGOUTPUT_IF_H_1
#define _LOGOUTPUT_IF_H_1

#include <set>
#include <map>
#include <sstream>    // for std::ostringstream
#include <functional> // for std::function
#include <assert.h>   // for assert
#include <iomanip>    // for std::setw
#include <memory>

#include "log/logmanager.h"

namespace log_tools {
// for class LogOutput
template<bool isUseLock>
struct UseLock {
	constexpr bool operator() (void) const {
		return isUseLock;
	}
};
} //  namespace log_tools

//
// class LogOutput_if
//
class LogOutput_if {
protected:
    // 同一个进程中只能使用一个子类
    // 参数 subclass_object 总是传入 单件子类的 this
    static bool is_unique_subclass_working(LogOutput_if* subclass_object) {
        assert(subclass_object);
        LogOutput_if::__set.insert(subclass_object);
        return LogOutput_if::__set.size() == 1;
    }
protected:
	class default_format {
	public:
		const std::string operator() (const std::shared_ptr<LogVal>& val) {
			std::ostringstream oss;
			oss << log_tools::time2string(val->now)
				<< " [" 
				<< std::right << std::setw(5)
				<< val->log_type
				<< "] " << val->msg << "\t[tid:" 
				<< val->tid << "] "
				<< val->file_name << ":" << val->line_num 
				<< ' ' << val->func_name 
				<< val->extra
				<< std::endl;
			return oss.str();
		}
	}; // class default_format
	typedef std::function<std::string(const std::shared_ptr<LogVal>&)> Format_t;
protected:
	inline static bool is_std_outerr(const std::ostream& os) {
		return (os == std::cout or os == std::cerr);
	}
	inline static boost::mutex* new_lock(const std::ostream& os) {
		return is_std_outerr(os) ? (&log_tools::print_lock()) 
			: (new boost::mutex());
	}
	void release_lock(const std::ostream& os, boost::mutex* lock) {
		if ((!is_std_outerr(os)) && lock) {
			delete lock;
			lock = NULL;
		}
	}
public:
    virtual ~LogOutput_if() {}

private:
    static std::set<LogOutput_if*> __set;
};


//// 
//// class LogOutput
//// 
//// log_tools::UseLock<true> : 
////     日志输出目的地时使用互斥锁, 避免多线程下对同一输出目标的争抢
//// log_tools::UseLock<false>: 
////     日志输出目的地时, 除std::cout和std::cerr外, 不使用互斥锁
////
//template <typename UseLock = log_tools::UseLock<true> >
//class LogOutput {
//private:
//	//class default_format {
//	//public:
//	//	const std::string operator() (const std::shared_ptr<LogVal>& val) {
//	//		std::ostringstream oss;
//	//		oss << log_tools::time2string(val->now)
//	//			<< " [" 
//	//			<< std::right << std::setw(5)
//	//			<< val->log_type
//	//			<< "] " << val->msg << "\t[tid:" 
//	//			<< val->tid << "] "
//	//			<< val->file_name << ":" << val->line_num 
//	//			<< ' ' << val->func_name 
//	//			<< val->extra
//	//			<< std::endl;
//	//		return oss.str();
//	//	}
//	//}; // class default_format
//	//typedef std::function<std::string(const std::shared_ptr<LogVal>&)> Format_t;
//
//	// 
//	// std::map[std::ostream] = os_property
//	//     key   => std::ostream
//	//     value => os_property
//	//
//	struct os_property {
//		boost::mutex* plock;
//        std::set<LogType>   loglevel_set;
//		Format_t      format;
//		os_property(boost::mutex* plock_ = NULL, 
//				const std::set<LogType>& loglevel_set_ = { 
//                /*
//                    makelevel(TRACE), makelevel(DEBUG),
//                    makelevel(INFO),  makelevel(WARN),
//                    makelevel(ERROR), makelevel(FATAL) 
//                */
//                    },
//				const Format_t& format_ = default_format()) :
//				 plock(plock_), loglevel_set(loglevel_set_), format(format_) {} 
//		os_property(boost::mutex* plock_ = NULL, 
//				std::set<LogType>&& loglevel_set_ = { 
//                /*
//                    makelevel(TRACE), makelevel(DEBUG),
//                    makelevel(INFO),  makelevel(WARN),
//                    makelevel(ERROR), makelevel(FATAL) 
//                */
//                    },
//				const Format_t& format_ = default_format()) :
//				 plock(plock_), loglevel_set(std::move(loglevel_set_)), format(format_) {} 
//		os_property(const os_property& that) :
//			plock(that.plock), loglevel_set(that.loglevel_set), 
//			format(that.format) {}
//	}; // struct os_property
//
//public:
//	//// scoped lock
//	//class scoped_lock {
//	//public:
//	//	scoped_lock(const std::ostream& os, boost::mutex& lock)
//	//			: _lock(lock), _os_is_std_outerr(LogOutput::is_std_outerr(os)) {
//	//		if (_os_is_std_outerr) {
//	//			_lock.lock();
//	//			return;	
//	//		}
//	//		if (LogOutput::_is_uselock()) {				
//	//			_lock.lock();
//	//			return;
//	//		}
//	//	}
//	//	~scoped_lock() {
//	//		if (_os_is_std_outerr) {
//	//			_lock.unlock();
//	//			return;
//	//		}
//	//		if (LogOutput::_is_uselock()) {				
//	//			_lock.unlock();
//	//			return;
//	//		}
//	//	}
//	//private:
//	//	boost::mutex& _lock;
//	//	bool          _os_is_std_outerr;
//	//}; // class scoped_lock
//private:
//	//inline static bool is_std_outerr(const std::ostream& os) {
//	//	return (os == std::cout or os == std::cerr);
//	//}
//	//inline static boost::mutex* new_lock(const std::ostream& os) {
//	//	return is_std_outerr(os) ? (&log_tools::print_lock()) 
//	//		: (new boost::mutex());
//	//}
//	//void release_lock(const std::ostream& os, boost::mutex* lock) {
//	//	if ((!is_std_outerr(os)) && lock) {
//	//		delete lock;
//	//		lock = NULL;
//	//	}
//	//}
//	
//private:
//	LogOutput(void)                        = default;
//public:
//	LogOutput(const LogOutput&)            = delete;
//	LogOutput& operator=(const LogOutput&) = delete;
//	~LogOutput() {
//		std::for_each(_outstream.begin(), _outstream.end(), 
//			[this] (std::pair<std::ostream*, os_property> element) -> void {
//				this->release_lock(*element.first, element.second.plock);
//			});
//	}
//public:
//	// 
//	inline static LogOutput& get_instance(void) {
//		static LogOutput instance;
//		return instance; 
//	}
//
//	// bind/insert ostream, ... 
//	template <typename Format = default_format>
//	bool bind(std::ostream& os, 
//			std::set<LogType>&& loglevel_set = { 
//                    makelevel(TRACE), makelevel(DEBUG),
//                    makelevel(INFO),  makelevel(WARN),
//                    makelevel(ERROR), makelevel(FATAL) 
//            }, // default bind TRACE, DEBUG, INFO, WARN, ERROR, FATAL
//			Format format = default_format()) {
//		boost::mutex::scoped_lock lock(_outstreamlock);
//		//os => {lock, loglevel_set, format}
//		auto ret = _outstream.insert(std::pair<std::ostream*, os_property>
//				(&os, os_property(new_lock(os), std::move(loglevel_set), format)));	
//		return ret.second; 
//	}
//
//	// unbind/remove ostream. 
//	// Just remove the ostream, but not destruct ostream
//	bool unbind(std::ostream& os) {
//		boost::mutex::scoped_lock lock(_outstreamlock);
//		boost::mutex* tmp_plock = _outstream[&os].plock;
//		auto ret = _outstream.erase(&os);
//		release_lock(os, tmp_plock);
//		return 1 == ret;
//	}
//
//	// only outputs one log record.
//	//     output the oldest log only once from LogStore.
//	//     If the LogStore is empty, it will wait until the
//	//     LogStore is not empty, and then output the logs.
//	void operator() (std::shared_ptr<LogVal>& val) {
//		LogStore::get_instance().pop(val);
//		boost::mutex::scoped_lock lock(_outstreamlock);
//		std::for_each(_outstream.begin(), _outstream.end(), 
//			[&val] (std::pair<std::ostream*, os_property> element) -> void {
//                auto& loglevel_set = element.second.loglevel_set;
//                if (loglevel_set.find(val->log_type) != loglevel_set.end())
//					scoped_lock lock(*element.first, *element.second.plock);
//					*element.first << element.second.format(val) << std::flush;
//				}
//			});
//	}
//
//    void output_once(std::shared_ptr<LogVal>& val) {
//        this->operator() (val);
//    }
//
//	// returns the specified lock pointer
//	// if the parameter, os, is not binding, it will return NULL
//	boost::mutex* get_lock(const std::ostream& os) const {
//		typedef boost::mutex mutex_t;
//		mutex_t::scoped_lock lock(const_cast<mutex_t&>(_outstreamlock));
//		for (auto it = _outstream.cbegin(); it != _outstream.cend(); ++it) {
//			if (it->first == &os) {
//				return it->second.plock;			
//			}
//		}
//		return NULL;
//	}
//
//	// Capacity
//	// bound ostream count
//	inline size_t ostream_count(void) const {
//		typedef boost::mutex mutex_t;
//		mutex_t::scoped_lock lock(const_cast<mutex_t&>(_outstreamlock));
//		return _outstream.size();
//	}
//	// when there are no ostreams, it is empty
//	inline bool empty() const {
//		typedef boost::mutex mutex_t;
//		mutex_t::scoped_lock lock(const_cast<mutex_t&>(_outstreamlock));
//		return _outstream.empty();
//	} 
//
//private:
//	boost::mutex                         _outstreamlock;
//	std::map<std::ostream*, os_property> _outstream;
//	//static UseLock                            _is_uselock; 
//	inline static bool _is_uselock(void) {
//		static UseLock is_uselock;
//		return is_uselock();
//	}
//}; // class LogOutput
//

#endif // _LOGOUTPUT_IF_H_1
