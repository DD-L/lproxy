#ifndef CONFIG_H_1
#define CONFIG_H_1
/*************************************************************************
	> File Name:    config.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/1 7:47:37
 ************************************************************************/

#include <crypto/encrytor.h>
#include <crypto/rsa_encryptor.h>
#include <lss/typedefine.h>

namespace lproxy {

class config {
    private:
      void configure (void) = 0;
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
    const data_t& get_server_name() {
        return m_server_name;
    }
    const data_t& get_server_name() {
        return m_server_port;
    }
    const data_t& get_auth_key() {
        return m_auth_key;
    }
    const bool get_zip_on(void) {
        return m_zip_on;
    }
private:
    virtual void configure() override {
        // read data from local configure file
        // TODO
        // 临时解决方案
        m_server_name = "192.168.33.124";
        m_server_port = "8088";
        m_auth_key    = "xxxxxxxxx";
        m_zip_on      = false;
    }
private:
    data_t m_server_name; /* server ipv4/ipv6/domain */
    data_t m_server_port; /* server port */
    data_t m_auth_key;    /* auth_key */

    bool                 m_zip_on = false;
    /*timeout in sec*/
}; 

} // namespace lproxy::local

namespace server {

// 饿汉单件
// class lproxy::server::config
class config : public lproxy::config {
private:
    config() : rsakey(config::m_rsakeysize) {
        configure();
    }
    config(const config&) = delete;
    config& operator= (const config&) = delete;

public:
    static config& get_instance() {
        static config instance;
        return instance;
    }
    constexpr crypto::RsaKey::size get_rsa_keysize(void) {
        return m_rsa_keysize;
    }
    const sdata_t& get_rsa_publickey_hex(void) {
        return m_rsakey.publicKeyHex;
    }
    const crypto::Rsakey& get_rsakey(void) {
        return m_rsakey;
    }
    const bool get_zip_on(void) {
        return m_zip_on;
    }
    const std::set<data_t>& get_cipher_auth_key_set(void) {
        return m_cipher_auth_key_set;
    }
private:
    virtual void configure() override {
        // read data from server configure file
        // TODO
        // 临时解决方案
        m_zip_on = false;
        // MD5(xxxxxxxxx)
        m_cipher_auth_key_set.insert("ABA369F7D2B28A9098A0A26FEB7DC965");
    }
private:
    static const 
    crypto::RsaKey::size m_rsa_keysize = crypto::RsaKey::bit1024;
    crypto::RsaKey       m_rsakey;
    bool                 m_zip_on = false;
    // 客户端 认证key 集合
    std::set<data_t>     m_cipher_auth_key_set;

    /*timeout in sec*/

}; // lproxy::server::config

} // namespace lproxy::server

} // namespace lproxy

#endif // CONFIG_H_1
