/**
 *  异常类
 */
#ifndef _EXCEPT_H
#define _EXCEPT_H
#include <string>
using std::string;

// 日志异常
class LogException : public std::exception {
public:
	LogException(void) noexcept;
	LogException(const string& msg) noexcept;
	virtual ~LogException (void) noexcept {}
	virtual const char* what() const noexcept;
private:
	std::string m_msg;
};

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
#endif // _EXCEPT_H
