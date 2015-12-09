/*******************
 *   异常类
 *******************/
#ifndef _EXCEPT_H_1
#define _EXCEPT_H_1
#include <string>

template<typename T>
class ExceptionTemplate : public std::exception {
public:
	ExceptionTemplate(void) noexcept : m_msg(T::name() + " Exception") {}
	ExceptionTemplate(const std::string& msg) noexcept
            : m_msg(T::name() + " Exception") {
        m_msg += msg;
    }
	virtual ~ExceptionTemplate (void) noexcept {}
	virtual const char* what() const noexcept {
        return m_msg.c_str();
    }
private:
	std::string m_msg;

}; // class ExceptionTemplate;


struct __LOG_Except {
    static std::string name() {
        return "Log";
    }
};

struct __CRYPTO_Except {
    static std::string name() {
        return "Crypto";
    }
};


// 日志异常
typedef ExceptionTemplate<__LOG_Except>    LogException;
// 加解密异常
typedef ExceptionTemplate<__CRYPTO_Except> CryptoException;


// 加密时异常
class EncryptException : public CryptoException {
public:
    EncryptException(void) noexcept;
    EncryptException(const std::string& msg) noexcept;
    virtual ~EncryptException(void) {}
}; // class EncryptException

// 解密时异常
class DecryptException : public CryptoException {
public:
    DecryptException(void) noexcept;
    DecryptException(const std::string& msg) noexcept;
    virtual ~DecryptException(void) {}
}; // class DecryptException



////////////////////////////////////////////////////
/*

// 服务器异常
class ServerException : public std::exception {
    private:
        string m_msg;
    public:
	ServerException (void);
	ServerException (const string& msg);
	virtual ~ServerException (void) throw ();
	virtual const char* what (void) const throw ();
};
// 线程异常
class ThreadException :
	public ServerException {
public:
	ThreadException (void);
	ThreadException (const string& msg);
};
// 网络异常
class SocketException :
	public ServerException {
public:
	SocketException (void);
	SocketException (const string& msg);
};
// HTTP 异常
class HTTPException : public ServerException {
public:
        HTTPException(void);
        HTTPException(const string& msg);
};
*/
#endif // _EXCEPT_H_1
