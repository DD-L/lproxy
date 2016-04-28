#ifndef CONFIG_SERVER_H_1
#define CONFIG_SERVER_H_1
/*************************************************************************
	> File Name:    config_server.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/28 7:47:37
 ************************************************************************/

#include <lss/config.h>
#include <crypto/encryptor.h>
#include <crypto/rsa_crypto.h>

namespace lproxy {
namespace server {

// class lproxy::server::config
class config : public lproxy::config {
private:
    config() : m_rsakey(config::m_rsa_keysize), m_logfile("") {};
    config(const config&) = delete;
    config& operator= (const config&) = delete;

public:
    virtual void configure(const sdata_t& config_file) override;
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
    const sdata_t& get_logfilename(void) {
        return m_logfile;
    }
    uint32_t get_timeout(void) {
        return m_timeout;
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

    uint32_t m_timeout = 90; /* in sec*/
    sdata_t  m_logfile;

}; // lproxy::server::config

} // namespace lproxy::server

} // namespace lproxy

#endif // CONFIG_SERVER_H_1


/*

server 配置示例:
{
    "lssserver": 
    {
        "bind_addr": "0.0.0.0",          // 本地绑定的 ip, 支持 ipv4/ipv6 
        "bind_port": 8088,               // 本地绑定的端口, 必须为数字类型
        "bind_addr_socks5": "127.0.0.1", // socks5 响应时返回的地址, 目前不需要更改. 支持 ipv4/ipv6/domain
        "bind_addr_type_socks5": "ipv4", // "bind_addr_socks5" 地址的类型, 可选值 [ "ipv4" | "ipv6" | "domain" ]
        "bind_port_socks5": 8088,        // socks5 响应时返回的端口，一般和 "bind_port" 保持一致. 必须为数字类型.
        "zip_on": false,                 // 是否对流量进行压缩. 布尔类型数据. 默认值 false
        "timeout": 30,                   // 超时设置 (秒). 必须为数字类型. 默认值 30
        //"logfile": "/var/log/lproxy/server.err", // (错误)运行日志. 如果未设置则不保存日志
        
        // lsslocal 的认证 key 集合. 计算方法: upper(md5-32(lsslocal.auth_key))
        "cipher_auth_key_set": [
            "ABA369F7D2B28A9098A0A26FEB7DC965", 
            "ABA369F7D2B28A9098A0A26FEB7DC966", 
            // ...,
            "ABA369F7D2B28A9098A0A26FEB7DC967"
        ]
    }
}

*/
