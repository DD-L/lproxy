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
class session : 
    public lproxy::session, public std::enable_shared_from_this<session> {
public:
    typedef std::shared_ptr<reply>   shared_reply_type;
    typedef std::shared_ptr<request> shared_request_type;
public:
    session(boost::asio::io_service& io_service_left,
                  boost::asio::io_service& io_service_right);

    /**
     * function:start {resolver_right.async_resolve [bind: resolve_handler]}
     */
    virtual void start(void) override;
    virtual tcp::socket& get_socket_left(void) override;
    virtual void close(void) throw() override;
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
            std::size_t bytes_transferred, shared_request_type lss_request);

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
     *          cut_lss
     *          if ('unprocessed data still in lss_reply') {
     *              async_write:socket_left [bind: right_read_handler]
     *          }
     *          else {
     *              async_write:socket_left [bind: left_write_handler]
     *          }
     *      }
     *      case (reply::deny) { close }
     *      case (reply::timeout) { close }
     *      case (reply::bad) { cloes }
     *      default { close }
     * }
     */
    void right_read_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred, shared_reply_type lss_reply,
            shared_data_type __data_right_rest, // 分包后，遗留的数据
            shared_data_type __write_data); // left_write 数据

    /**
     * function:left_read_socks5_step1 {
     *      async_write:socket_left [bind: left_write_socks5_step1_handler]
     * }
     */
    void left_read_socks5_step1(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_data_type data_left);

    /**
     * function:left_write_socks5_step1_handler {
     *      socket_left.async_read_some  [bind: left_read_handler]
     * }
     */
    void left_write_socks5_step1_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred, shared_data_type __data);
private:
    /**
     * function:transport {
     *      socket_left.async_read_some  [bind: left_read_socks5_step1]
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
            std::size_t bytes_transferred, shared_data_type data_left);

    /**
     * function:right_write_handler {
     *      socket_left.async_read_some [bind: left_read_handler]
     * }
     */
    void right_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred, shared_request_type lss_request);

    /**
     * function:left_write_handler {
     *      socket_right.async_read_some [bind: right_read_handler]
     * }
     */
    void left_write_handler(const boost::system::error_code& error,
            std::size_t bytes_trannsferred, 
            shared_data_type __data/*=lproxy::placeholders::shared_data*/);

    /**
     * function:left_read_timeout_handler { this->close } 
     */
    void left_read_timeout_handler(const boost::system::error_code& error);

private:
    // 组装 hello
    const request& pack_hello(void);
    // 组装 exchange
    const request pack_exchange(const keysize_t& keysize, 
            const data_t& public_key);
    
    // 组装 data / zipdata
    const request pack_data(const data_t& data_left, 
            const std::size_t data_len);
    
    // 组装 bad
    const request& pack_bad(void);
private:
    // 解包hello. 获取pack.data中的 keysize 与 public_key
    void unpack_reply_hello(keysize_t& keysize, 
            data_t& public_key, const reply& reply);

    // 解包exchange. 获取 随机key 和 随机数
    void unpack_reply_exchange(sdata_t& reply_random_str, 
            const reply& reply);

    // 解包data.
    const int unpack_reply_data(data_t& data_right, std::size_t lss_length, 
            const reply& reply, bool is_zip = false);

private:
    inline shared_reply_type make_shared_reply(void) {
        return std::make_shared<reply>(max_length);
    }
    inline shared_request_type make_shared_request(const request& lss_request) {
        return std::make_shared<request>(std::move(lss_request));
    }
private:
    // 消灭 全局reply
    //lproxy::local::reply   lss_reply; // server 端发来的原始数据
    // 消灭 全局 request
    //lproxy::local::request lss_request;// 向server端发送的数据 (hello, bad 除外)
    sdata_t                  random_str; // local 端生成的随机数
    std::shared_ptr<crypto::Encryptor> aes_encryptor;
    vdata_t     data_key;  // server 端发来的 随机 key, 也是 数据传输 用的 key
    // 消灭全局 data_left
    //vdata_t     data_left; //从 client 发来的数据（通常是 socks5 数据） 
private:
    tcp::socket      socket_left;     // client socket
    tcp::socket      socket_right;    // server socket
    tcp::resolver    resolver_right;  // server resolver
    std::atomic_flag close_flag = ATOMIC_FLAG_INIT;
    boost::mutex     close_mutex;

}; // class lproxy::local::session

} // namespace lproxy::local
} // namespace lproxy

#endif // _SESSION_LOCAL_H_1
