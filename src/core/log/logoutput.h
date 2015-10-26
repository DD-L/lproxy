#ifndef _LOGOUTPUT_H_1
#define _LOGOUTPUT_H_1

#include <map>
#include <sstream>    // for std::ostringstream
#include <functional> // for std::function
#include <assert.h>   // for assert
#include <iomanip>    // for std::setw
//#include "log/init_simple.h"
//#include "log/logmanager.h"

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
// class LogOutput
// 
// log_tools::UseLock<true> : 
//     日志输出目的地时使用互斥锁, 避免多线程下对同一输出目标的争抢
// log_tools::UseLock<false>: 
//     日志输出目的地时, 除std::cout和std::cerr外, 不使用互斥锁
//
template <typename UseLock = log_tools::UseLock<true> >
class LogOutput {
private:
	class default_format {
	public:
		const std::string operator() (const std::shared_ptr<LogVal>& val) {
			std::ostringstream oss;
			oss << log_tools::time2string(val->now)
				<< " [" 
				<< std::right << std::setw(5)
				<< val->log_type
				<< "] " << val->msg << "\t[pid:" 
				<< val->pid << "] "
				<< val->file_name << ":" << val->line_num 
				<< ' ' << val->func_name 
				<< val->extra
				<< std::endl;
			return oss.str();
		}
	}; // class default_format
	typedef std::function<std::string(const std::shared_ptr<LogVal>&)> Format_t;

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
	
private:
	LogOutput(void)                        = default;
public:
	LogOutput(const LogOutput&)            = delete;
	LogOutput& operator=(const LogOutput&) = delete;
	~LogOutput() {
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
}; // class LogOutput


#endif // _LOGOUTPUT_H_1