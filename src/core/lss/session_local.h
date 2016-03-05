#ifndef _SESSION_LOCAL_H_1
#define _SESSION_LOCAL_H_1
/*************************************************************************
	> File Name:    session_local.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/1 3:56:39
 ************************************************************************/
#include <atomic>
#include <lss/session.h>

namespace lproxy {
namespace local {

/**
 * class lproxy::local::session
 */ 
class session : public lproxy::session {
public:
    session(boost::asio::io_service& io_service_left,
                  boost::asio::io_service& io_service_right);

    /**
     * function:start {resolver_right.async_resolve [bind: resolve_handler]}
     */
    virtual void start(void) override;
    virtual tcp::socket& socket_left(void) override;
private:
    /**
     * function:resolve_handler {
     *      async_connect:socket_right [bind: connect_handler]
     * }
     */
    void resolve_handler(const boost::system::error_code& ec,
            tcp::resolver::iterator i);

    /**
     * function:connect_handler {
     *      case (! ec) {
     *          async_write:socket_right [bind: hello_handler]
     *      }
     *      case (i != tcp::resolver::iterator()) {
     *          socket_right.async_connect [bind: connect_handler]
     *      }
     * } 
     */
    void connect_handler(const boost::system::error_code& ec,
            tcp::resolver::iterator i);
    /*
    void shakehands_right_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);
    */

    /**
     * function:hello_handler {
     *      socket_right.async_read_some [bind: right_read_handler]
     * }
     */
    void hello_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);
    /**
     * function:right_read_handler {
     *      case (reply::hello) {
     *          unpack_reply_hello
     *          pack_exchange
     *          async_write:socket_right [bind: exchange_handler]
     *      }
     * }
     */
    void right_read_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);

    void exchange_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);
private:
    void transport(void);

private:
    void left_read_handler(const boost::system::error_code& error,
            size_t bytes_transferred);

    void right_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);
    void left_write_handler(const boost::system::error_code& error,
            std::size_t bytes_trannsferred);

private:
    // 组装 hello
    const request& pack_hello(void);
    // 组装 exchange
    const request pack_exchange(const data_t& data);
    
    // 组装 data
    //const request pack_data(const data_t& data);
    const request pack_data(const data_t& data, std::size_t data_len);
    
    // 组装 zipdata
    //const request pack_zipdata(const data_t& data);
    // 组装 bad
    const request& pack_bad(void);
private:
    // 获取pack.data中的 keysize 与 public_key
    void unpack_reply_hello(keysize_t& keysize, data_t& public_key);

private:
    void parse_hello_reply(keysize_t& keysize, data_t& public_key);

    // 获取随机key, 并验证服务端
    bool parse_exchange();

private:
    void delete_this(void);
private:
    /*
    enum {
        status_not_connected = 0,
        status_connected     = 1,
        status_hello         = 2,
        status_auth          = 3,
        status_data          = 4
    } status; 
    */
    lproxy::local::reply    lss_reply; // server 端发来的原始数据
    const std::string& auth_key = "xxxxxx"; // 明文
    std::string        random_str; // local 端生成的随机数
    bool               zip_on = false;
    std::shared_ptr<crypto::Encryptor> aes_encryptor;
    std::string        data_key;  // server 端发来的 随机 key, 也是 数据传输 用的 key
    enum             { max_length = 2048 };
    //uint8_t          data_left[max_length]; // 从 client 发来的 数据 （通常是 socks5 数据）
    std::vector<uint8_t> data_left; //从 client 发来的 数据 （通常是 socks5 数据） 
private:
    tcp::socket      socket_left;  // client 
    tcp::socket      socket_right; // remote
    //tcp::endpoint  remote_ep; // ??????
    //tcp::resolver    resolver_right;
    std::atomic_flag delete_flag = ATOMIC_FLAG_INIT;
}; // class lproxy::local::session

} // namespace lproxy::local
} // namespace lproxy

#endif // _SESSION_LOCAL_H_1
