#ifndef CONFIG_LOCAL_H_1
#define CONFIG_LOCAL_H_1
/*************************************************************************
	> File Name:    config_local.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/28 7:47:37
 ************************************************************************/

#include <lss/config.h>

namespace lproxy {
namespace local {

class config : public lproxy::config {
private:
    config() : m_logfile("") {};
    config(const config&) = delete;
    config& operator= (const config&) = delete;
public:
    virtual void configure(const sdata_t& config_file) override;
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
    const sdata_t& get_bind_addr(void) {
        return m_bind_addr;
    }
    const sdata_t& get_logfilename(void) {
        return m_logfile;
    }
    uint32_t get_timeout(void) {
        return m_timeout;
    }
private:
    sdata_t m_server_name; /* server ipv4/ipv6/domain */
    sdata_t m_server_port; /* server port */
    sdata_t m_auth_key;    /* auth_key */

    bool                 m_zip_on = false;

    uint16_t m_bind_port; // 本地绑定的端口
    sdata_t  m_bind_addr; // ipv4 or ipv6. 本地绑定的地址
    uint32_t m_timeout = 30; /* in sec*/
    sdata_t  m_logfile;
}; 

} // namespace lproxy::local
} // namespace lproxy

#endif // CONFIG_LOCAL_H_1


/*

local 配置示例:
{
    "lsslocal":
    {
        "bind_addr": "0.0.0.0",     // 本地绑定的 ip, 支持 ipv4/ipv6
        "bind_port": 8087,          // 本地绑定的端口, 必须为数字类型
        "server_name": "127.0.0.1", // lssserver 的地址, 支持ipv4/ipv6/domain
        "server_port": "8088",      // lssserver 的端口, 字符串类型
        "auth_key": "xxxxxxxxx",    // lsslocal 端的认证 key, 字符串类型
        "zip_on": false,            // 是否对流量进行压缩. 布尔类型数据. 默认值 false
        //"logfile": "/var/log/lproxy/local.err", // (错误)运行日志. 如果未设置则不保存日志
        "timeout": 30               // 超时设置 (秒). 必须为数字类型. 默认值 30
    }
}

*/
