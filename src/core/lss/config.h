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

class config : public lproxy::config {}; 

} // namespace lproxy::local

namespace server {

// 懒汉单件
// class lproxy::server::config
class config : public lproxy::config {
private:
    config() : rsakey(config::m_rsakeysize) {}
    config(const config&) {};

public:
    static config& get_instance() {
        static config instance;
        return instance;
    }
    constexpr crypto::RsaKey::size get_rsa_keysize(void) {
        return m_rsa_keysize;
    }
    std::string& get_rsa_publickey_hex(void) {
        return m_rsakey.publicKeyHex;
    }
    crypto::Rsakey& get_rsakey(void) {
        return m_rsakey;
    }
private:
    static const crypto::RsaKey::size m_rsa_keysize = crypto::RsaKey::bit1024;
    crypto::RsaKey m_rsakey;

}; // lproxy::server::config

} // namespace lproxy::server

} // namespace lproxy

#endif // CONFIG_H_1
