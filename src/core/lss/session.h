#ifndef _SESSION_H_1
#define _SESSION_H_1
/*************************************************************************
	> File Name:    session.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 15:58:54
 ************************************************************************/
#include <type_traits>
#include <lss/typedefine.h>
#include <lss/lss_packet.h>
namespace lproxy {

class session {
public:
    typedef std::shared_ptr<session>  pointer;
public:
    virtual void start(void) = 0;
    virtual void close(void) = 0;
    virtual tcp::socket& get_socket_left(void) = 0;
    virtual ~session(void) {
        // debug
        std::cout << "session::~session() this = " << this << std::endl;
    }
protected:
    //enum             { max_length = 2048};
    enum             { max_length = 1024};
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
    };
    class incomplete_data : public std::exception {
    public:
        incomplete_data(int less) noexcept : less_(less) {}
        virtual ~incomplete_data(void) noexcept {}
        virtual const char* what(void) const noexcept {
            return "incomplete_data";
        }
        const int less(void) const {
            return less_;
        }
    private:
        int   less_;
    };
protected:
    // 包完整性检查
    void lss_pack_integrity_check(std::size_t bytes_transferred, 
            const request_and_reply_base_class& lss_data) 
                throw (incomplete_data) {
        if (bytes_transferred < 4) {
            throw incomplete_data(0xffffffff);
        }
        int less = lss_data.data_len() + 4 - bytes_transferred;
        if (less > 0) {
            throw incomplete_data(less);
        }
    }

    // 裁剪 lss 包数据 (分包)
    /**
     * @brief cut_lss
     * @param start_pos  [std::size_t] 开始切割的位置
     * @param lss_len    [std::size_t] 当前 lss 包有效长度
     * @param lss [lproxy::local::reply | lproxy::server::request] 要出里的 lss 
     * @return  [bool] 切割后剩下的数据包是否完整。true 完整，false 不完整 
     */
    template<typename LSSTYPE> 
    bool cut_lss(const std::size_t start_pos, 
            const std::size_t lss_len, LSSTYPE& lss) {
        static_assert(std::is_same<LSSTYPE, lproxy::local::reply>::value || 
            std::is_same<LSSTYPE, lproxy::server::request>::value, 
            "lss type must be 'lproxy::local::reply' "
            "or 'lproxy::server::request' !");
        
        if (lss_len < start_pos + 4) { // lss 包头部分长度为4
            return false; 
            // 如果切割后，剩下的数据包会不完整, 所以切割无意义，
            // 多余的丢掉不处理即可。
            // 调用者需检查返回值，如果 false，
            // 切记要改用另外一个 write bind handler
        }
        vdata_t&& buf = lproxy::get_vdata_from_lss_pack(lss);
        auto&& pack = lproxy::__packet(
                *(buf.begin() + start_pos),
                *(buf.begin() + start_pos + 1),
                *(buf.begin() + start_pos + 2),
                *(buf.begin() + start_pos + 3), // 包头部分结束
                data_t(buf.begin() + start_pos + 4, buf.begin() + lss_len));
        lss = LSSTYPE(std::move(pack));
        return true;
    }

}; // class lproxy::session


/**
 * class lproxy::random_string
 *
 * lproxy::sdata_t&& random_number_str = random_string::generate_number();
 * lproxy::vdata_t&& random_str        = random_string::generate();
 */
class random_string {
public:
    static sdata_t generate_number(const std::size_t length = 10) {
        init();
        byte random_array[length];
        for (std::size_t i = 0; i < length; ++i) {
            random_array[i] = ::rand() % 10 + '0';
        }
        return sdata_t(random_array, random_array + length);
    }
    static vdata_t generate(const std::size_t length = 10) {
        init();
        byte random_array[length];
        for (std::size_t i = 0; i < length; ++i) {
            random_array[i] = ::rand() % 255;
        }
        return vdata_t(random_array, random_array + length);
    }
private:
    class __random {
    public:
        __random(void) {
            ::srand((unsigned)time(NULL));
        }
    }; // class __random
    static void init(void) {
        static __random r; (void)r;
    }
}; // class lproxy::random_string

} // namespace lproxy
#endif // _SESSION_H_1
