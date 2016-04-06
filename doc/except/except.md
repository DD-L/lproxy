except/except.h

-----

# ExceptionTemplate

```cpp
template<typename T>
class ExceptionTemplate : public std::exception;
```

异常模板类

### 类摘要

```cpp
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
```

# LogException

日志异常类

```cpp
struct __LOG_Except {
    static std::string name() {
        return "Log";
    }
};
typedef ExceptionTemplate<__LOG_Except>    LogException;
```

# CryptoException

加解密异常类

```cpp
struct __CRYPTO_Except {
    static std::string name() {
        return "Crypto";
    }
};
typedef ExceptionTemplate<__CRYPTO_Except> CryptoException;
```

# EncryptException

加密时异常

```cpp
class EncryptException : public CryptoException {
public:
    EncryptException(void) noexcept : CryptoException() {}
    EncryptException(const std::string& msg) noexcept : CryptoException(msg) {}
    virtual ~EncryptException(void) {}
}; // class EncryptException
```

# DecryptException

解密时异常

```cpp
class DecryptException : public CryptoException {
public:
    DecryptException(void) noexcept : CryptoException() {}
    DecryptException(const std::string& msg) noexcept : CryptoException(msg) {}
    virtual ~DecryptException(void) {}
}; // class DecryptException
```

