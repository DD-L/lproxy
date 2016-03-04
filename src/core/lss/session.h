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
protected:
    //reply  lss_reply; // server 端 或 local 端 发来的原始数据
protected:
    enum {
        status_not_connected = 0,
        status_connected     = 1,
        status_hello         = 2,
        status_auth          = 3,
        status_data          = 4
    } status; 
    class wrong_packet_type : public std::exception {
    public:
        wrong_packet_type(void) noexcept : std::exception() {}
        virtual ~wrong_packet_type(void) noexcept {}
        virtual const char* what(void) const noexcept {
            return "wrong_packet_type";
        } 
    }
    class incomplete_data : public std::exception {
    public:
        incomplete_data(int less) noexcept : less_(less) {}
        virtual ~incomplete_data(void) noexcept {}
        virtual const char* what(void) const noexcept {
            return "incomplete_data";
        }
        const int less(void) {
            return less_;
        }
        const int less(void) const {
            return const_cast<decltype(this)>(this)->less();
        }
    private:
        uint16_t    less_;
    }
protected:
}; // class lproxy::session


/**
 * class lproxy::random_string
 *
 * std::string&& random_number_str = random_string::generate_number();
 * std::string&& random_str        = random_string::generate();
 */
class random_string {
public:
    static std::string generate_number(const uint32_t length = 10) {
        init();
        byte random_array[length];
        for (int i = 0; i < length; ++i) {
            random_array[i] = ::rand() % 10 + '0';
        }
        return std::string(random_array, random_array + length);
    }
    static std::string generate(const uint32_t length = 10) {
        init();
        byte random_array[length];
        for (int i = 0; i < length; ++i) {
            random_array[i] = ::rand() % 255;
        }
        return std::string(random_array, random_array + length);
    }
private:
    class __random {
    public:
        __random(void) {
            ::srand((unsigned)time(NULL));
        }
    }; // class __random
    static void init(void) {
        static __random r; void(r);
    }
}; // class lproxy::random_string

} // namespace lproxy
#endif // _SESSION_H_1
