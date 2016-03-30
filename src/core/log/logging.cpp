#include "log/logging.h"
#include "log/init_simple.h"
#include "except/except.h"

void logging (
		const LogType& log_type, const std::string& msg, 
		const LogVal::tid_t& tid, const std::string& func_name, 
		const std::string& file_name, const unsigned int& line_num, 
		std::shared_ptr<LogVal::Extra> extra 
			// = std::make_shared<LogVal::ExtraNone>()
	) {
	/*
	LogStore::get_instance().push ({
			log_tools::local_time(), log_type, msg,
			tid, func_name, file_name, line_num, extra
	});
	*/
	try {
		LogStore::get_instance().push( new LogVal (
				log_tools::local_time(), log_type, msg,
				tid, func_name, file_name, line_num, extra
		));
		// 这里new出来的内存会交给后面的std::share_ptr来管理(释放)
	}
	catch (const std::bad_alloc& e) {
		std::ostringstream msg;
		msg << "Memory allocation failure, when a log is being generated."
			" [" << __func__ << " @" << __FILE__ << ':' << __LINE__ << "] "
			"e.what() = " << e.what();
		throw LogException(msg.str());	
	}
}

void logging (
		const LogType& log_type, std::string&& msg,
		const LogVal::tid_t& tid, std::string&& func_name, 
		std::string&& file_name, const unsigned int& line_num, 
		std::shared_ptr<LogVal::Extra> extra 
			// = std::make_shared<LogVal::ExtraNone>()
	) {
	/*
	LogStore::get_instance().push ({
			log_tools::local_time(), log_type, std::move(msg),
			tid, std::move(func_name), std::move(file_name), line_num, extra
	});
	*/
	try {
		LogStore::get_instance().push( new LogVal (
				log_tools::local_time(), log_type, std::move(msg),
				tid, std::move(func_name), std::move(file_name), line_num, extra
		));
		// 这里new出来的内存会交给后面的std::share_ptr来管理(释放)
	}
	catch (const std::bad_alloc& e) {
		std::ostringstream msg;
		msg << "Memory allocation failure, when a log is being generated."
			" [" << __func__ << " @" << __FILE__ << ':' << __LINE__ << "] "
			"e.what() = " << e.what();
		throw LogException(msg.str());	
	}
}



