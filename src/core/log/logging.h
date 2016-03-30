#ifndef LOG_H__0
#define LOG_H__0
#include "log/log_types.h"

void logging (
		const LogType& log_type, const std::string& msg, 
		const LogVal::tid_t& tid, const std::string& func_name, 
		const std::string& file_name, const unsigned int& line_num, 
		std::shared_ptr<LogVal::Extra> extra 
			= std::make_shared<LogVal::ExtraNone>()
);

void logging (
		const LogType& log_type, std::string&& msg,
		const LogVal::tid_t& tid, std::string&& func_name, 
		std::string&& file_name, const unsigned int& line_num, 
		std::shared_ptr<LogVal::Extra> extra 
			= std::make_shared<LogVal::ExtraNone>()
);

#include <sstream>

// trace
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

// debug
#define logdebug(log) \
		do {std::ostringstream __oss;\
		(__oss << log);\
		logging(makelevel(DEBUG), std::move(__oss.str()), \
				log_tools::get_tid(),\
				__func__, __FILE__, __LINE__);} while (0)
#define logdebugEx(log, shared_ptr_extra) \
		do {std::ostringstream __oss;\
		(__oss << log);\
		logging(makelevel(DEBUG), std::move(__oss.str()), \
				log_tools::get_tid(),\
				__func__,__FILE__, __LINE__, shared_ptr_extra);} while (0)

// info
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

// warn
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

// error
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

// fatal
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

#endif // LOG_H__0
