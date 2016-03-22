#ifndef CONFIG_H_1
#define CONFIG_H_1
/*************************************************************************
	> File Name:    config.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/1 7:47:37
 ************************************************************************/
#include <crypto/encryptor.h>
#include <crypto/rsa_crypto.h>
#include <lss/typedefine.h>
#include <lss/log.h>
#include <thread>
#include <chrono>

namespace lproxy {

class config {
    virtual void configure(void) = 0;
public:
    virtual ~config(void) {}
    static void signal_handler(boost::asio::io_service* ios,
            const boost::system::error_code& err, 
            int signal) {
        if (err)   return;
        if (! ios) return;
        switch (signal) {
            case SIGINT:
            case SIGTERM:
                _print_s("Exit..");
                ios->stop();
                while (! ios->stopped()) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                exit(0);
        default:
            break;
        } 
    } 
}; // class config

namespace local {

class config : public lproxy::config {
private:
    config() /*: xxx*/ {
        configure();
    }
    config(const config&) = delete;
    config& operator= (const config&) = delete;
public:
    static config& get_instance() {
        static config instance;
        return instance;
    }
    const sdata_t& get_server_name() {
        return m_server_name;
    }
    const sdata_t& get_server_port() {
        return m_server_port;
    }
    const sdata_t& get_auth_key() {
        return m_auth_key;
    }
    const bool get_zip_on(void) {
        return m_zip_on;
    }
    uint16_t get_bind_port(void) {
        return m_bind_port;
    }
    sdata_t& get_bind_addr(void) {
        return m_bind_addr;
    }
    sdata_t get_logfilename(void) {
        return "";
    }
    uint32_t get_timeout(void) {
        return m_timeout;
    }
private:
    virtual void configure() override {
        // read data from local configure file
        // TODO
        // 临时解决方案
        //m_server_name = /*(const_byte_ptr)*/"192.168.33.124";
        m_server_name = /*(const_byte_ptr)*/"127.0.0.1";
        m_server_port = /*(const_byte_ptr)*/"8088";
        m_auth_key    = /*(const_byte_ptr)*/"xxxxxxxxx";
        m_zip_on      = false;
        m_bind_port = 8087;
        m_bind_addr = "0.0.0.0";
        m_timeout  = 30; // in sec
    }
private:
    sdata_t m_server_name; /* server ipv4/ipv6/domain */
    sdata_t m_server_port; /* server port */
    sdata_t m_auth_key;    /* auth_key */

    bool                 m_zip_on = false;

    uint16_t m_bind_port; // 本地绑定的端口
    sdata_t  m_bind_addr; // ipv4 or ipv6. 本地绑定的地址
    uint32_t m_timeout = 30; /* in sec*/
}; 

} // namespace lproxy::local

namespace server {

// class lproxy::server::config
class config : public lproxy::config {
private:
    config() : m_rsakey(config::m_rsa_keysize) {
        configure();
    }
    config(const config&) = delete;
    config& operator= (const config&) = delete;

public:
    static config& get_instance() {
        static config instance;
        return instance;
    }
    //crypto::RsaKey::size get_rsa_keysize(void) {
    uint16_t get_rsa_keysize(void) {
        return (uint16_t)m_rsa_keysize;
    }
    const sdata_t get_rsa_publickey_hex(void) {
        return m_rsakey.publicKeyHex();
    }
    const crypto::RsaKey& get_rsakey(void) {
        return m_rsakey;
    }
    const bool get_zip_on(void) {
        return m_zip_on;
    }
    const std::set<data_t>& get_cipher_auth_key_set(void) {
        return m_cipher_auth_key_set;
    }

    const sdata_t& get_bind_addr(void) {
        return m_bind_addr;
    }
    uint16_t get_bind_port(void) {
        return m_bind_port;
    }
    const sdata_t& get_bind_addr_socks5(void) {
        return m_bind_addr_socks5;
    }
    uint16_t get_bind_port_socks5(void) {
        return m_bind_port_socks5;
    }
    const sdata_t& get_bind_addr_type_socks5(void) {
        return m_bind_addr_type_socks5;
    }
    sdata_t get_logfilename(void) {
        return "";
    }
    uint32_t get_timeout(void) {
        return m_timeout;
    }
private:
    virtual void configure() override {
        // read data from server configure file
        // TODO
        // 临时解决方案
        m_zip_on = false;
        // MD5-32-upper(xxxxxxxxx)
        m_cipher_auth_key_set.insert(
                (const_byte_ptr)"ABA369F7D2B28A9098A0A26FEB7DC965");

        //m_bind_addr = "192.168.33.124";
        m_bind_addr = "0.0.0.0"; // ipv4 or ipv6. 本地绑定ip
        m_bind_port = 8088;      // 本地绑定 port
        
        // socks5 服务器 应答 resp 所用的字段
        // VER REP RSV ATYP BND-ADDR BND-PORT
        m_bind_addr_socks5      = "127.0.0.1"; // 服务器地址, 一般不需要更改.
                                               //   支持 ipv4, ipv6, domain
        m_bind_addr_type_socks5 = "ipv4";      // 一般不需要更改. 
                                               //   可选值 [ipv4|ipv6|domain]
        m_bind_port_socks5      = 8088;        // 一般 和 m_bind_port 设置相同

        m_timeout  = 30; // in sec
    }
private:
    static const 
    crypto::RsaKey::size m_rsa_keysize = crypto::RsaKey::bit1024;
    crypto::RsaKey       m_rsakey;
    bool                 m_zip_on = false;
    // 客户端 认证 key 集合. (简易内存数据库)
    std::set<data_t>     m_cipher_auth_key_set;

    sdata_t  m_bind_addr;
    uint16_t m_bind_port;
    sdata_t  m_bind_addr_socks5;
    sdata_t  m_bind_addr_type_socks5;
    uint16_t m_bind_port_socks5;

    uint32_t m_timeout = 30; /* in sec*/

}; // lproxy::server::config

} // namespace lproxy::server

} // namespace lproxy

#endif // CONFIG_H_1
