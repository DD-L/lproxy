#ifndef CONFIG_H_1
#define CONFIG_H_1
/*************************************************************************
	> File Name:    config.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/1 7:47:37
 ************************************************************************/

#include "crypto/encrytor.h"
#include "crypto/rsa_encryptor.h"

namespace lproxy {

class config {}; // class config

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
    const std::string& get_server_name() {
        return m_server_name;
    }
    const std::string& get_server_name() {
        return m_server_port;
    }
    const std::string& get_auth_key() {
        return m_auth_key;
    }
private:
    void configure() {
        // read data from configure file
        // ...
        // TODO
        // 临时解决方案
        m_server_name = "192.168.33.124";
        m_server_port = "8088";
        m_auth_key    = "xxxxxxxxx";
    }
private:
    std::string m_server_name; /* server ipv4/ipv6/domain */
    std::string m_server_port; /* server port */
    std::string m_auth_key;    /* auth_key */
}; 

} // namespace lproxy::local

namespace server {

// 饿汉单件
// class lproxy::server::config
class config : public lproxy::config {
private:
    config() : rsakey(config::m_rsakeysize) {}
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
    const std::string& get_rsa_publickey_hex(void) {
        return m_rsakey.publicKeyHex;
    }
    const crypto::Rsakey& get_rsakey(void) {
        return m_rsakey;
    }
private:
    static const 
    crypto::RsaKey::size m_rsa_keysize = crypto::RsaKey::bit1024;
    crypto::RsaKey       m_rsakey;

}; // lproxy::server::config

} // namespace lproxy::server

} // namespace lproxy

#endif // CONFIG_H_1
