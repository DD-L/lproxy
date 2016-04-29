#ifndef _SESSION_SERVER_H_1
#define _SESSION_SERVER_H_1
/*************************************************************************
	> File Name:    session_server.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/1 3:59:10
 ************************************************************************/
#include <lss/session.h>
#include <lss/socks5_protocol.h>
#include <crypto/encryptor.h>

namespace lproxy {
namespace server {

class session : 
    public lproxy::session, public std::enable_shared_from_this<session> {
public:
    typedef std::shared_ptr<request> shared_request_type;
    typedef std::shared_ptr<reply>   shared_reply_type;
public:
    session(boost::asio::io_service& io_service_left,
            boost::asio::io_service& io_service_right);
    virtual ~session(void);
    /**
     * function:start {socket_left.async_read_some [bind: left_read_handler]}
     */
    virtual void start(void) override;
    virtual tcp::socket& get_socket_left(void) override;
private:
    virtual void close(void) throw() override;
    virtual void cancel(void) throw() override;
private:
    /**
     * function:left_read_handler {
     *      case (request::hello) {
     *          async_write:socket_left [bind: hello_handler]
     *      }
     *      case (request::exchange) {
     *          unpack_request_exchange
     *          case ('authenticate key' failed) {
     *              async_write:socket_left (reply::deny) [bind: close]
     *          }
     *          case ('authenticate key' succeeded) {
     *              async_write:socket_left (reply::exchange) {
     *                  [bind: exchange_handler]
     *              }
     *          }
     *      }
     *      case (request::zipdata or request::data) {
     *          unpack_data
     *          cut_lss
     *          // sock5
     *          // https://github.com/DD-L/lproxy/issues/127
     *          //case (socks5::server::OPENING) {
     *          //   'VER NMETHODS METHODS' -> socks5::ident_req
     *          //   'VER NMETHOD' -> sock5::ident_resp
     *          //   sock5::ident_resp -> rply_data 
     *          //   async_write:socket_left [bind: left_write_handler]
     *          //}
     *          if (socks5::server::OPENING)
     *              this->socks5_state = lproxy::socks5::server::CONNECTING
     *          case (socks5::server::CONNECTING) {
     *              'VER CMD RSV ATYP DST-ADDR DST-PROT' -> socks5::req rq
     *              socks5_request_processing(rq);
     *              socket_left.async_read_some [bind: left_read_handler]
     *          }
     *          case (socks5::server::CONNECTED) {
     *              case (CMD_CONNECT) {
     *                  if ('unprocessed data still in lss_request') {
     *                      async_write:this->sock_right_tcp {
     *                          [bind: left_read_handler]
     *                      }
     *                  }
     *                  else {
     *                      async_write:this->socket_right_tcp {
     *                          [bind: right_write_handler]
     *                      }
     *                  }
     *              }
     *              case (CMD_BIND) {
     *                  // TODO
     *              }
     *              case (CMD_UDP) {
     *                  if ('unprocessed data still in lss_request') {
     *                      socket_right_udp.async_send_to {
     *                          [bind: left_read_handler]
     *                  }
     *                  else {
     *                      socket_right_udp.async_send_to {
     *                          [bind: right_write_handler]
     *                  }
     *              }
     *          }
     *      }
     * }
     */
    void left_read_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred, shared_request_type lss_reply,
            shared_data_type __data_left_rest, shared_data_type __write_data);
    /**
     * function:hello_handler {
     *  socket_left.async_read_some [bind: left_read_handler]
     * }
     */
    void hello_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred);

    /**
     * function:exchange_handler {
     *      socket_left.async_read_some [bind: left_read_handler]
     * }
     */
    void exchange_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred, shared_reply_type lss_reply);

    /**
     * function:left_write_handler {
     *      if (lproxy::socks5::server::CONNECTED == this->socks5_state) {
     *          case (CMD_CONNECT) {
     *              socket_right_tcp.async_read_some [bind: right_read_handler]
     *          }
     *          case (CMD_BIND) {
     *              // TODO
     *          }
     *          case (CMD_UDP) {
     *              socket_right_udp.async_receive_from {
     *                  [bind: right_read_handler]
     *              }
     *          }
     *          default {
     *              async_write:socket_left (pack_bad().buffers()) {
     *                  [bind: close]
     *              }
     *          }
     *      }
     *      else {
     *          socket_left.async_read_some [bind: left_read_handler]
     *      }
     * }
     */
    void left_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred, shared_reply_type lss_reply); 
    
    /**
     * function: right_write_handler {
     *      case (CMD_CONNECT) {
     *          socket_left.async_read_some [bind: left_read_handler]
     *          
     *          //socket_right_tcp.async_read_some {
     *          //    [bind: right_read_handler]
     *          //}
     *      }
     *      case (CMD_BIND) {
     *          // TODO
     *      }
     *      case (CMD_UDP) {
     *          socket_left.async_read_some [bind: left_read_handler]
     *
     *          //socket_right_udp.async_receive_from {
     *          //    [bind: right_read_handler]
     *          //}
     *      }
     *      default {
     *          async_write:socket_left (pack_bad().buffers()) {
     *              [bind: close]
     *          }
     *      }
     * }
     */
    void right_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred, 
            shared_data_type __data/* = lproxy::placeholders::shared_data*/);

    /**
     * function:right_read_handler {
     *      pack_data
     *      async_write:socket_left [bind: left_write_handler]
     * }
     */
    void right_read_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred, shared_data_type data_right);

    /**
     * funciton:right_read_timeout_handler {
     *      if (error != boost::asio::error::operation_aborted) {
     *          async_write:this->socket_left [bind: close]
     *      }
     *      else {
     *          this->close();
     *      }
     * }
     */
    void right_read_timeout_handler(const boost::system::error_code& error);
private:
    // 组装 hello
    const reply& pack_hello(void);
    // 组装 deny
    const reply& pack_deny(void);
    // 组装 exchange
    const reply pack_exchange(const data_t& auth_key, 
            const data_t& random_str);
    // 组装 data
    const reply pack_data(const std::string& data, std::size_t data_len);
    const reply pack_data(const data_t& data, std::size_t data_len);
    // 组装 bad
    const reply& pack_bad(void);
    // 组装 timeout
    const reply& pack_timeout(void);

private:
    const data_t gen_hello_data(void);
    void unpack_request_exchange(data_t& auth_key, data_t& random_str,
            const request& request);

    /**
     * function:unpack_data {
     *      if ('this->aes_encryptor not initialized') {
     *          async_write:socket_left (deny.buffers()) {
     *              [bind: close]
     *          }
     *      }
     *      else {
     *          'unzip if requested'
     *          'decrypt'
     *      }
     * }
     */
     const int unpack_data(data_t& plain, const std::size_t lss_reply_length, 
             const request& request, bool is_zip = false);

    // socks5 [var cmd rsv atype dstaddr dstport] 处理流程
    /**
     * function:socks5_request_processing {
     *     case (0x01) 'ADDR-TYPE ipv4' {
     *          this->dest_name = 
     *     }
     *     case (0x03) 'ADDR-TYPE domain' {
     *          this->dest_name = 
     *     }
     *     case (0x04) 'ADDR-TYPE ipv6' {
     *          this->dest_name = 
     *     }
     *     default { throw illegal_data_type }
     *
     *     this->dest_port = rq.DestPort
     *
     *     case (0x01) 'CMD CONNECT' {
     *          this->socks5_cmd = CMD_CONNECT
     *          resovle_connect_tcp(this->dest_name, this->dest_port)
     *     }
     *     case (0x02) 'CMD BIND' {
     *          TODO
     *     }
     *     case (0x03) 'CMD UDP' {
     *          this->socks5_cmd = CMD_UDP
     *          resovle_open_udp(this->dest_name, this->dest_port)
     *     }
     *     default { '0x07 CMD_UNSUPPORT' }
     *
     *     if (Cmd != 0x01 && Cmd != 0x03) {
     *          socks5_resp_to_local
     *     }
     * }
     */
    void socks5_request_processing(const lproxy::socks5::req& rq);

    /**
     * function:resovle_connect_tcp {
     *     ip::tcp::resolver::query::async_resolve {
     *          [bind: tcp_resolve_handler]
     *     }
     * }
     */
    void resovle_connect_tcp(const char* name, uint16_t port);

    /**
     * function:resovle_open_udp {
     *    ip::udp::resolver::query::async_resolve {
     *          [bind: udp_resolve_handler]
     *    }
     * }
     */
    void resovle_open_udp(const char* name, uint16_t port);

    /**
     * function:tcp_resolve_handler {
     *      socket_right_tcp.async_connect {
     *          [bind: tcp_connect_handler]
     *      }
     * }
     */
    void tcp_resolve_handler(const boost::system::error_code& err, 
        tcp::resolver::iterator endpoint_iterator);

    /**
     * function:udp_resolve_handler {
     *      socket_right_udp.async_connect {
     *          [bind: udp_connect_handler]
     *      }
     * }
     */
    void udp_resolve_handler(const boost::system::error_code& err, 
        udp::resolver::iterator endpoint_iterator);

    /**
     * function:tcp_connect_handler {
     *      case ('connected') {
     *          this->socks5_resp_reply = 0x00
     *          socks5_resp_to_local
     *      }
     *      case ('connection failed, try next') {
     *          socket_right_tcp.async_connect [bind: tcp_connect_handler]
     *      }
     *      case ('connection failed, end') {
     *          this->socks5_resp_reply = 0x03
     *          socks5_resp_to_local
     *      }
     * }
     */
    void tcp_connect_handler(const boost::system::error_code& err,
      tcp::resolver::iterator endpoint_iterator);

    /**
     * function:udp_connect_handler {
     *      case ('connected') {
     *          this->dest_name =
     *          socket_right_udp.open
     *          socks5_resp_to_local
     *      }
     *      case ('connection failed, try next') {
     *          socket_right_udp.async_connect [bind: tcp_connect_handler]
     *      }
     *      case ('connection failed, end') {
     *          this->socks5_resp_reply = 0x03
     *          socks5_resp_to_local
     *      }
     * }
     */
    void udp_connect_handler(const boost::system::error_code& err,
          udp::resolver::iterator endpoint_iterator);
    

    // 封包 socks5::resp 数据包
    // TODO
    data_t& pack_socks5_resp(data_t& data);
    
    // 将socks5::resp反馈给local
    /**
     * function:socks5_resp_to_local {
     *      pack_socks5_resp
     *      async_write:socket_left [bind: left_write_handler]
     *      if (this->socks_resp_reply == 0x00) {
     *          this->socks5_state = lproxy::socks5::server::CONNECTED;
     *      }
     * }
     */
    void socks5_resp_to_local();
private:
    inline shared_request_type make_shared_request(void) {
        return std::make_shared<request>(max_length);
    }
    inline shared_reply_type make_shared_reply(const reply& lss_reply) {
        return std::make_shared<reply>(std::move(lss_reply));
    }
private:
    tcp::socket       socket_left; // local
    tcp::socket       socket_right_tcp; // remote tcp
    udp::socket       socket_right_udp; // remote udp
    std::shared_ptr<tcp::resolver> resolver_right_tcp;// 
    std::shared_ptr<udp::resolver> resolver_right_udp;// 
    boost::asio::deadline_timer    timer_right;

    enum {
        CMD_CONNECT   = 0x01,
        CMD_BIND      = 0x02,
        CMD_UDP       = 0x03,
        CMD_UNSUPPORT = 0xff
    } socks5_cmd; // socks5_cmd 
    sdata_t        dest_name; // 目标ip(v4/v6)或域名
    uint16_t       dest_port; // 目标端口
    uint8_t        socks5_resp_reply; // socks5::resq::Reply

    // 消灭 全局lss_request
    //lproxy::server::request lss_request; // 从local端发来的原始数据
    // 消灭 全局lss_reply
    //lproxy::server::reply   lss_reply;   // 发向 local 的数据(hello, bad, timeout, deny 除外)
    // 消灭全局 data_right
    //data_t             data_right; // 从web 发来的原始数据, 
    //不用数组是为了减少lproxy::server::session的对象构造所用的时间

    lproxy::socks5::server::state socks5_state = lproxy::socks5::server::OPENING;

    std::shared_ptr<crypto::Encryptor> aes_encryptor;
    //std::atomic_flag                   close_flag = ATOMIC_FLAG_INIT;
    //boost::mutex                       close_mutex;
}; // class lproxy::server::session
} // namespace lproxy::server
} // namespace lproxy

#endif // _SESSION_SERVER_H_1
