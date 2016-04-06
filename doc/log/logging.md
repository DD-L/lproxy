log/logging.h

----

# 日志生成工具



```cpp
void logging (
		const LogType& log_type,       const std::string& msg, 
		const LogVal::tid_t& tid,      const std::string& func_name, 
		const std::string& file_name,  const unsigned int& line_num, 
		std::shared_ptr<LogVal::Extra> extra 
			= std::make_shared<LogVal::ExtraNone>()
);

void logging (
		const LogType& log_type,       std::string&& msg,
		const LogVal::tid_t& tid,      std::string&& func_name, 
		std::string&& file_name,       const unsigned int& line_num, 
		std::shared_ptr<LogVal::Extra> extra 
			= std::make_shared<LogVal::ExtraNone>()
);

```

并且内置了 6 种级别的 快速格式化宏函数

* trace

	1. logtrace(log);
	2. logtraceEx(log, shared_ptr_extra);

	```cpp
	#define logtrace(log) \
			do {std::ostringstream __oss;\
			(__oss << log);\
			logging(makelevel(TRACE), std::move(__oss.str()), \
					log_tools::get_tid(),\
					__func__, __FILE__, __LINE__);} while (0)
	#define logtraceEx(log, shared_ptr_extra) \
			do {std::ostringstream __oss;\
			(__oss << log);\
			logging(makelevel(TRACE), std::move(__oss.str()), \
					log_tools::get_tid(),\
					__func__,__FILE__, __LINE__, shared_ptr_extra);} while (0)
	```


* debug
	
	1. logdebug(log);
	2. logdebugEx(log, shared_ptr_extra);

	```cpp
	#define loginfo(log) \
		do {std::ostringstream __oss;\
		(__oss << log);\
		logging(makelevel(INFO), std::move(__oss.str()), \
				log_tools::get_tid(),\
				__func__, __FILE__, __LINE__);} while (0)
	#define loginfoEx(log, shared_ptr_extra) \
			do {std::ostringstream __oss;\
			(__oss << log);\
			logging(makelevel(INFO), std::move(__oss.str()), \
					log_tools::get_tid(),\
					__func__,__FILE__, __LINE__, shared_ptr_extra);} while (0)
	```

* warn

	1. logwarn(log);
	2. logwarnEx(log, shared_ptr_extra);

	```cpp
	#define logwarn(log) \
		do {std::ostringstream __oss;\
		(__oss << log);\
		logging(makelevel(WARN), std::move(__oss.str()), \
				log_tools::get_tid(),\
				__func__, __FILE__, __LINE__);} while (0)
	#define logwarnEx(log, shared_ptr_extra) \
			do {std::ostringstream __oss;\
			(__oss << log);\
			logging(makelevel(WARN), std::move(__oss.str()), \
					log_tools::get_tid(),\
					__func__,__FILE__, __LINE__, shared_ptr_extra);} while (0)
	```

* error

	1. logerror(log)
	2. logerrorEx(log, shared_ptr_extra)

	```cpp
	#define logerror(log) \
			do {std::ostringstream __oss;\
			(__oss << log);\
			logging(makelevel(ERROR), std::move(__oss.str()), \
					log_tools::get_tid(),\
					__func__, __FILE__, __LINE__);} while (0)
	#define logerrorEx(log, shared_ptr_extra) \
			do {std::ostringstream __oss;\
			(__oss << log);\
			logging(makelevel(ERROR), std::move(__oss.str()), \
					log_tools::get_tid(),\
					__func__,__FILE__, __LINE__, shared_ptr_extra);} while (0)
	```

* fatal

	1. logfatal(log)
	2. logfatalEx(log, shared_ptr_extra);

	```cpp
	#define logfatal(log) \
			do {std::ostringstream __oss;\
			(__oss << log);\
			logging(makelevel(FATAL), std::move(__oss.str()), \
					log_tools::get_tid(),\
					__func__, __FILE__, __LINE__);} while (0)
	#define logfatalEx(log, shared_ptr_extra) \
			do {std::ostringstream __oss;\
			(__oss << log);\
			logging(makelevel(FATAL), std::move(__oss.str()), \
					log_tools::get_tid(),\
					__func__,__FILE__, __LINE__, shared_ptr_extra);} while (0)
	```
