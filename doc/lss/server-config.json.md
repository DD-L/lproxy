# server 端配置文件

根据需要更改配置

*下面是默认的配置*
```javascript
{
    "lssserver": 
    {
        "bind_addr": "0.0.0.0",          // 本地绑定的 ip 地址, 支持 ipv4/ipv6 地址类型
        "bind_port": 8088,               // 本地监听的端口, 必须为数字类型
        "bind_addr_socks5": "127.0.0.1", // socks5 响应时返回的地址, 目前不需要更改. 支持 ipv4/ipv6/domain
        "bind_addr_type_socks5": "ipv4", // "bind_addr_socks5" 地址的类型, 可选值 [ "ipv4" | "ipv6" | "domain" ]
        "bind_port_socks5": 8088,        // socks5 响应时返回的端口，一般和 "bind_port" 保持一致. 必须为数字类型.
        "zip_on": false,                 // 是否对流量进行压缩. 布尔类型数据. 如果未设置则默认值为 false
        "timeout": 90,                   // 超时设置 (秒). 必须为数字类型. 如果未设置则默认值为 90
        //"logfile": "/var/log/lproxy/server.err", // (错误)运行日志. 如果未设置则不保存日志
        
        // lsslocal 的认证 key 集合, 重复无效. 
        //  计算方法: (32 字节长度的大写 md5 结果)
        //      upper(md5-32(lsslocal.auth_key))
        //  用 python 来描述是:
        //     >>> import hashlib
        //     >>> auth_key = "xxxxxxxxx"
        //     >>> hashlib.md5(auth_key.encode()).hexdigest().upper()
        //     'ABA369F7D2B28A9098A0A26FEB7DC965'
        //     >>>
        "cipher_auth_key_set": [
            //"62B1021B8A7BB09630BEF739189DDDF8",  // "abdeF1"
            // ...,
            "ABA369F7D2B28A9098A0A26FEB7DC965"    // "xxxxxxxxx"
        ]
    }
}
```