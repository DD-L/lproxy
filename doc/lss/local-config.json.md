# local 端配置文件

根据需要更改配置

*下面是默认的配置*
```javascript
{
    "lsslocal":
    {
        "bind_addr": "0.0.0.0",     // 本地绑定的 ip 地址, 支持 ipv4/ipv6 地址类型
        "bind_port": 8087,          // 本地监听的端口, 必须为数字类型
        "server_name": "127.0.0.1", // lssserver 端的地址, 支持 ipv4/ipv6/domain 类型
        "server_port": "8088",      // lssserver 端的端口, 字符串类型
        "auth_key": "xxxxxxxxx",    // lsslocal 端的认证 key, 字符串类型
        "zip_on": false,            // 是否对流量进行压缩. 布尔类型数据. 如果未设置则默认值为 false
        //"logfile": "/var/log/lproxy/local.err", // (错误)运行日志. 如果未设置则不保存日志
        "timeout": 30               // 超时设置 (秒). 必须为数字类型. 如果未设置则默认值为 30
    }
}
```