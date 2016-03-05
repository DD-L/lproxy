/*******************
 *   异常类
 *******************/
#ifndef _EXCEPT_H_1
#define _EXCEPT_H_1
#include <string>

template<typename T>
class ExceptionTemplate : public std::exception {
public:
	ExceptionTemplate(void) noexcept : m_msg(T::name() + " Exception: ") {}
	ExceptionTemplate(const std::string& msg) noexcept
            : m_msg(T::name() + " Exception: ") {
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
    EncryptException(void) noexcept : CryptoException() {}
    EncryptException(const std::string& msg) noexcept : CryptoException(msg) {}
    virtual ~EncryptException(void) {}
}; // class EncryptException

// 解密时异常
class DecryptException : public CryptoException {
public:
    DecryptException(void) noexcept : CryptoException() {}
    DecryptException(const std::string& msg) noexcept : CryptoException(msg) {}
    virtual ~DecryptException(void) {}
}; // class DecryptException


#endif // _EXCEPT_H_1
