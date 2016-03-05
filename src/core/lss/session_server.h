#ifndef _SESSION_SERVER_H_1
#define _SESSION_SERVER_H_1
/*************************************************************************
	> File Name:    session_server.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/1 3:59:10
 ************************************************************************/
#include "lss/session.h"

namespace lproxy {
namespace server {

class session : public lproxy::session {
public:
    session(boost::asio::io_service& io_service_left,
            boost::asio::io_service& io_service_right);
    /**
     * function:start {socket_left.async_read_some [bind: left_read_handler]}
     */
    virtual void start(void) override;
    virtual tcp::socket& socket_left(void) override;
private:
    /**
     * function:left_read_handler {
     *      case (request::hello) {
     *          async_write:socket_left [bind: hello_handler]
     *      }
     * }
     */
    void left_read_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);
    /**
     * function:hello_handler {
     *  socket_left.async_read_some [bind: left_read_handler]
     * }
     */
    void hello_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);
    void exchange_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);

    //void left_write_handler 【leftread -> left_read_handler】
    void left_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred); 
    
    //void right_write_handler 【rightread -> right_read_handler】
    void right_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);
    //void right_read_handler  【leftwrite -> left_write_handler】
    void right_read_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);

private:
    void delete_this(void);
private:
    // 组装 hello
    const reply& pack_hello(void);
    // 组装 deny
    const reply& pack_deny(void);
    // 组装 exchange
    const reply pack_exchange(const std::string& auth_key,
            const std::string& random_str);
    // 组装 data
    const reply pack_data(const std::string& data, std::size_t data_len);
    const reply pack_data(const lproxy::socks5::data_t& data,
            std::size_t data_len);
    // 组装 bad
    const reply& pack_bad(void);
    // 组装 timeout
    const reply& pack_timeout(void);

private:
    std::string gen_hello_data(void);
    void get_authkey_randomstr(std::string& auth_key, std::string& random_str);
    std::string& get_plain_data(std::string& plain);

    // socks5 [var cmd rsv atype dstaddr dstport] 处理流程
    void socks5_request_processing(const lproxy::socks5::req& rq);
    void resovle_connect_tcp(const char* name, uint16_t port);
    void resovle_open_udp(const char* name, uint16_t port);
    void tcp_resolve_handler(const boost::system::error_code& err, 
        tcp::resolver::iterator endpoint_iterator);
    void upd_resolve_handler(const boost::system::error_code& err, 
        tcp::resolver::iterator endpoint_iterator);

    void tcp_connect_handler(const boost::system::error_code& err,
      tcp::resolver::iterator endpoint_iterator);

    // 封包 socks5::resp 数据包
    lproxy::data_t& pack_socks5_resp(lproxy::data_t& data);
    // 将socks5::resp反馈给local
    void socks5_resp_to_local();
private:
    tcp::socket       socket_left; // local
    tcp::socket       socket_right_tcp; // remote tcp
    udp::socket       socket_right_udp; // remote udp
    bool              zip_on = false; // config

    enum {
        CMD_CONNECT   = 0x01,
        CMD_BIND      = 0x02,
        CMD_UDP       = 0x03,
        CMD_UNSUPPORT = 0xff
    } socks5_cmd; // socks5_cmd 
    std::string    dest_name; // 目标ip(v4/v6)或域名
    uint16_t       dest_port; // 目标端口
    uint8_t        socks5_resp_reply; // socks5::resq::Reply

    lproxy::server::request lss_request; // 从local端发来的原始数据
    //std::string       data_right; // 从web 发来的原始数据
    enum             { max_length = 2048};
    //uint8_t          data_right[max_length]; // 从web 发来的原始数据
    std::vector<uint8_t> data_right; // 从web 发来的原始数据, 
    //不用数组是为了减少lproxy::server::session的对象构造所用的时间

    lprxoy::socks5::server::state socks5_state = lprxoy::socks5::server::OPENING;

    std::shared_ptr<crypto::Encryptor> aes_encryptor;
    std::atomic_flag  delete_flag = ATOMIC_FLAG_INIT;
}; // class lproxy::server::session
} // namespace lproxy::server
} // namespace lproxy

#endif // _SESSION_SERVER_H_1
