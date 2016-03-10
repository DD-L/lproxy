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
#include <crypto/aes_crypto.h>

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
    virtual tcp::socket& get_socket_left(void) override;
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

    /**
     * function:hello_handler {
     *      socket_right.async_read_some [bind: right_read_handler]
     * }
     */
    void hello_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);

    /**
     * function:exchange_handler {
     *      socket_right.async_read_some [bind: right_read_handler]
     * }
     */
    void exchange_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);

    /**
     * function:right_read_handler {
     *      case (reply::hello) {
     *          unpack_reply_hello
     *          pack_exchange
     *          async_write:socket_right [bind: exchange_handler]
     *      }
     *      case (reply::exchange) {
     *          unpack_reply_exchange
     *          'verify_random_string'
     *          transport
     *      }
     *      case (reply::zipdata or reply::data) {
     *          unpack_reply_data
     *          async_write:socket_left [bind: left_write_handler]
     *      }
     *      case (reply::deny) { delete_this }
     *      case (reply::timeout) { delete_this }
     *      case (reply::bad) { delete_this }
     *      default { delete_this }
     * }
     */
    void right_read_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);

private:
    /**
     * function:transport {
     *      socket_left.async_read_some  [bind: left_read_handler]
     *      socket_right.async_read_some [bind: right_read_handler]
     * }
     */
    void transport(void);

private:
    /**
     * function:left_read_handler {
     *      pack_data
     *      async_write:socket_right [bind: right_write_handler]
     * }
     */
    void left_read_handler(const boost::system::error_code& error,
            size_t bytes_transferred);

    /**
     * function:right_write_handler {
     *      socket_left.async_read_some [bind: left_read_handler]
     * }
     */
    void right_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);

    /**
     * function:left_write_handler {
     *      socket_right.async_read_some [bind: right_read_handler]
     * }
     */
    void left_write_handler(const boost::system::error_code& error,
            std::size_t bytes_trannsferred);

private:
    // 组装 hello
    const request& pack_hello(void);
    // 组装 exchange
    const request pack_exchange(const keysize_t& keysize, 
            const data_t& public_key);
    
    // 组装 data / zipdata
    const request pack_data(std::size_t data_len);
    
    // 组装 bad
    const request& pack_bad(void);
private:
    // 解包hello. 获取pack.data中的 keysize 与 public_key
    void unpack_reply_hello(keysize_t& keysize, data_t& public_key);

    // 解包exchange. 获取 随机key 和 随机数
    void unpack_reply_exchange(sdata_t& reply_random_str);

    // 解包data.
    void unpack_reply_data(data_t& data_right, bool is_zip = false);

private:
    void delete_this(void);
private:
    lproxy::local::reply    lss_reply; // server 端发来的原始数据
    sdata_t                  random_str; // local 端生成的随机数
    std::shared_ptr<crypto::Encryptor> aes_encryptor;
    vdata_t                   data_key;  // server 端发来的 随机 key, 也是 数据传输 用的 key
    enum             { max_length = 2048 };
    vdata_t          data_left; //从 client 发来的数据（通常是 socks5 数据） 
private:
    tcp::socket      socket_left;  // client 
    tcp::socket      socket_right; // remote
    tcp::resolver resolver_right;  // remote resolver
    std::atomic_flag delete_flag = ATOMIC_FLAG_INIT;
}; // class lproxy::local::session

} // namespace lproxy::local
} // namespace lproxy

#endif // _SESSION_LOCAL_H_1
