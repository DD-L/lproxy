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
#include <thread>
#include <chrono>

namespace lproxy {

class config {
    virtual void configure(void) = 0;
public:
    static void signal_handler(boost::asio::io_service* ios,
            const boost::system::error_code& err, 
            int signal) {
        if (err)   return;
        if (! ios) return;
        switch (signal) {
            case SIGINT:
            case SIGTERM:
                std::cout << "Exit..\n";
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

// 饿汉单件
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
    uint16_t get_local_bind_port(void) {
        return m_local_bind_port;
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
        m_local_bind_port = 8087;
    }
private:
    sdata_t m_server_name; /* server ipv4/ipv6/domain */
    sdata_t m_server_port; /* server port */
    sdata_t m_auth_key;    /* auth_key */

    bool                 m_zip_on = false;
    /*timeout in sec*/

    uint16_t m_local_bind_port;
}; 

} // namespace lproxy::local

namespace server {

// 饿汉单件
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
    const sdata_t& get_bind_addr_type(void) {
        return m_bind_addr_type;
    }
    uint16_t get_bind_port(void) {
        return m_bind_port;
    }
private:
    virtual void configure() override {
        // read data from server configure file
        // TODO
        // 临时解决方案
        m_zip_on = false;
        // MD5(xxxxxxxxx)
        m_cipher_auth_key_set.insert(
                (const_byte_ptr)"ABA369F7D2B28A9098A0A26FEB7DC965");

        //m_bind_addr = "192.168.33.124";
        m_bind_addr = "127.0.0.1";
        m_bind_addr_type = "ipv4";
        m_bind_port = 8088;
    }
private:
    static const 
    crypto::RsaKey::size m_rsa_keysize = crypto::RsaKey::bit1024;
    crypto::RsaKey       m_rsakey;
    bool                 m_zip_on = false;
    // 客户端 认证key 集合
    std::set<data_t>     m_cipher_auth_key_set;

    sdata_t  m_bind_addr;
    sdata_t  m_bind_addr_type;
    uint16_t m_bind_port;

    /*timeout in sec*/

}; // lproxy::server::config

} // namespace lproxy::server

} // namespace lproxy

#endif // CONFIG_H_1
