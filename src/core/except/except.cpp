// except.cpp

#include "except/except.h"
//#include "charcodecast.h"
//extern CCC ccc;

using std::string;


// 明文加密时异常
EncryptException::EncryptException(void) noexcept : CryptoException() {}
EncryptException::EncryptException(const std::string& msg) noexcept 
    : CryptoException(msg) {}

// 密文解密时异常
DecryptException::DecryptException(void) noexcept : CryptoException() {}
DecryptException::DecryptException(const std::string& msg) noexcept
    : CryptoException(msg) {}




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
