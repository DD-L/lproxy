#ifndef _SESSION_H_1
#define _SESSION_H_1
/*************************************************************************
	> File Name:    session.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 15:58:54
 ************************************************************************/

namespace lproxy {

using boost::asio::ip::tcp;
using boost::asio::ip::udp;

class session {
public:
    virtual void start(void) = 0;
    virtual tcp::socket& socket(void) = 0;
}; // class session

} // namespace lproxy
#endif // _SESSION_H_1
