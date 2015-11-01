// except.cpp

#include "except.h"
//#include "charcodecast.h"
//extern CCC ccc;

using std::string;

// 日志异常
LogException::LogException(void) noexcept : m_msg("Log Exception") {}
LogException::LogException(const string& msg) noexcept 
		: m_msg("Log Exception: ") {
	m_msg += msg;
}
const char* LogException::what(void) const noexcept {
	return m_msg.c_str();
}

/*
// 服务器异常 类实现
ServerException::ServerException(void) : m_msg(ccc("服务器异常！")) {}
ServerException::ServerException (const string& msg) :
	m_msg(ccc("服务器异常：")) {
	m_msg += msg;
	m_msg += "！";
}
ServerException::~ServerException(void) throw () {}
const char* ServerException::what(void) const throw () {
	return m_msg.c_str ();
}

// 线程异常 类实现
ThreadException::ThreadException(void) :
	ServerException(ccc("线程错误")) {}
ThreadException::ThreadException(const string& msg) :
	ServerException(msg) {}

// 网络异常 类实现

SocketException::SocketException (void) :
	ServerException (ccc("网络错误")) {}
SocketException::SocketException(const string& msg) :
	ServerException (msg) {}


// HTTP 异常 类实现
HTTPException::HTTPException(void) : 
    ServerException(ccc("HTTP 异常")) {}
HTTPException::HTTPException(const string& msg) :
    ServerException(msg) {}
*/
