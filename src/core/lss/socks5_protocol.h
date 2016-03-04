#ifndef SOCKS5_PROTOCOL_H_1
#define SOCKS5_PROTOCOL_H_1
/*************************************************************************
	> File Name:    socks5_protocol.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/2 6:15:08
 ************************************************************************/

// https://www.ietf.org/rfc/rfc1928.txt
//
//
// A label may contain zero to 63 characters. The null label, of length zero, 
// is reserved for the root zone. The full domain name may not exceed the length
// of 253 characters in its textual representation. In the internal binary 
// representation of the DNS the maximum length requires 255 octets of storage, 
// since it also stores the length of the name.
//

#include <assert.h>
#include <stdint.h>
#include <vector>
#include <exception>

namespace lproxy {

namespace socks5 {

typedef std::vector<uint8_t> data_t;
const uint8_t _version_ = 5;

// 不支持的socks版本
class unsupported_version : public std::exception {
public:
    unsupported_version(void) noexcept : std::exception() {}
    virtual ~unsupported_version(void) noexcept {}
    virtual const char* what(void) const noexcept {
        return "socks5: Unsupported_Socks_Version";
    } 
};

// 非法的数据类型
class illegal_data_type : public std::exception {
public:
    illegal_data_type(void) noexcept : std::exception() {}
    virtual ~illegal_data_type(void) noexcept {}
    virtual const char* what(void) const noexcept {
        return "socks5: illegal_data_type";
    } 
};

// lproxy::socks5::server
namespace server {
enum state {
    OPENING = 0,
    CONNECTING,
    CONNECTED
};
} // namspace lproxy::socks5::server

struct ident_req {
    uint8_t Version = 0x05;
    uint8_t NumberOfMethods;
    //uint8_t Methods[256];
    data_t  Methods;
    ident_req(const uint8_t* data_p, std::size_t len) {
        if (!(data_p && (len >= 3))) {
            throw illegal_data_type();
        }
        Version         = *(data_p + 0);
        if (Version != _version_) { 
            throw unsupported_version();
        }
        NumberOfMethods = *(data_p + 1);
        Methods.assign(data_p + 2, data_p + 2 + NumberOfMethods);
        assert(Methods.size() < 256);
    }
};

struct ident_resp {
    uint8_t Version = 0x05;
    uint8_t Method;
    static data_t& pack(data_t& data, ident_req* ident_req_p = nullptr) {
        if (ident_req_p) {
            Version = ident_req_p->Version; // always 5
            // 从 ident_req_p->Methods 中选择一个
            for (auto& v : ident_req_p->Methods) {
                // 当前仅支持 0x00 的认证方法
                if (v == 0x00) {
                    Method  = 0x00; 
                    uint8_t data_arr[2] = {0x05, 0x00}; 
                    data.assign(data_arr, data_arr + 2);
                    return data;
                }
            }
            // 没有一个认证方法被选中，客户端需要关闭连接
            uint8_t data_arr[2] = {0x05, 0xff};
            data.assign(data_arr, data_arr + 2);
        }
        else {
            // 默认方法
            uint8_t data_arr[2] = {0x05, 0x00}; 
            data.assign(data_arr, data_arr + 2);
        }
        return data;
    } 
};

struct req {
    uint8_t Version = 0x05;
    //enum Version { V5 = 0x05 } version;
    uint8_t Cmd;
    //enum Cmd { CONNECT = 0x01, BIND = 0x02, UDP = 0x03 } cmd;
    uint8_t Reserved = 0x00;
    //enum Reserved { RESERVED = 0x00 } reserved;
    uint8_t AddrType;
    //enum AddrType {IPv4 = 0x01, DOMAIN = 0x03, IPV6 = 0x04} addrType;
    union {
        uint8_t IPv4[4];
        struct {
            uint8_t Len;
            //uint8_t name[256];
            std::string name;
        } Domain;
        uint8_t IPv6[16];
    } DestAddr;
    uint16_t DestPort;
    req(const uint8_t* data_p, std::size_t len) {
        if (!(data_p && (len < 7))) {
            throw illegal_data_type();
        }
        Version         = *(data_p + 0);
        if (Version != _version_) { 
            throw unsupported_version();
        }
        Cmd             = *(data_p + 1);
        if (Cmd != 0x01 && Cmd != 0x02 && Cmd != 0x03) {
            throw illegal_data_type();
        }
        Reserved        = *(data_p + 2);    
        if (Reserved != 0x00) {
            throw illegal_data_type();
        }
        AddrType        = *(data_p + 3);

        uint8_t port_high_byte_ = 0, port_low_byte_ = 0;
        switch(AddrType) {
        case 0x01: // ipv4
            memmove(DestAddr.IPv4, data_p + 4, 4);
            
            port_high_byte_ = *(data_p + 8);
            port_low_byte_  = *(data_p + 9);
            
            break;
        case 0x03: {
            DestAddr.Domain.Len = *(data_p + 4);
            char name[DestAddr.Domain.Len];
            ::memmove(name, data_p + 5, DestAddr.Domain.Len);
            //memset(DestAddr.Domain.name, 0, 256);
            //memmove(DestAddr.Domain.name, data_p + 5, 
            //        DestAddr.Domain.Len);
            DestAddr.Domain.name.assign(name, name + DestAddr.Domain.Len);

            port_high_byte_ = *(data_p + 6 + DestAddr.Domain.Len);
            port_low_byte_  = *(data_p + 7 + DestAddr.Domain.Len);
            break;
        }
        case 0x04: 
            memmove(DestAddr.IPv6, data_p + 4, 16);

            port_high_byte_ = *(data_p + 20);
            port_low_byte_  = *(data_p + 21);
            break;
        default: throw illegal_data_type();
        }
        
        DestPort = (port_high_byte_ << 8) & 0xff00;
        DestPort = DestPort | port_low_byte_;
    }

};

struct resp {
    uint8_t Version = 0x05;
    uint8_t Reply;
    uint8_t Reserved = 0x00;
    uint8_t AddrType;
    union {
        uint8_t IPv4[4];
        struct {
            uint8_t Len;
            std::string name;
        } Domain;
        uint8_t IPv6[16];
    } BindAddr;
    uint16_t  BindPort;

    // method
    void set_IPv4(const uint8_t* ip, std::size_t size = 4) {
        assert(size == 4);
        AddrType = 0x01;
        ::memmove(BindAddr.IPv4, ip, 4);
    }
    void set_IPv6(const uint8_t* ip, std::size_t size = 16) {
        assert(size == 16);
        AddrType = 0x04;
        ::memmove(BindAddr.IPv4, ip, 16);
    }
    void set_Domain(const std::string& name) {
        assert(name.size() < 256);
        AddrType = 0x03;
        BindAddr.Domain.Len = name.size();
        BindAddr.Domain.name = name;
    }
    void set_BindPort(const uint16_t port) {
        BindPort = port;
    }

    data_t& pack(data_t& data) {

        const std::size_t size = 4+1+256+2;
        uint8_t data_arr[size] = {0}; 
        
        data_arr[0] = 0x05;
        
        data_arr[1] = Reply;
        switch (Reply) {
        case 0x00: // 成功
        case 0x01: // 普通SOCKS服务器连接失败
        case 0x02: // 现有规则不允许连接
        case 0x03: // 网络不可达
        case 0x04: // 主机不可达
        case 0x05: // 连接被拒
        case 0x06: // TTL超时
        case 0x07: // 不支持的命令
        case 0x08: // 不支持的地址类型
            break;
        default: //0x09 - 0xFF 未定义
            throw illegal_data_type();
        }
        
        data_arr[2] = 0x00;
        
        data_arr[3] = AddrType;
        switch (AddrType) {
        case 0x01: // ipv4
            memmove(data_arr + 4, BindAddr.IPv4, 4);
            data_arr[8] = (BindPort >> 8) & 0xff;
            data_arr[9] = BindPort & 0xff;

            data.assign(data_arr, data_arr + 10);
            break;
        case 0x03: // domain
            if (6 + BindAddr.Domain.Len >= size) {
                throw illegal_data_type();
            }

            data_arr[4] = BindAddr.Domain.Len;
            memmove(data_arr + 5, BindAddr.Domain.name.c_str(), 
                    BindAddr.Domain.Len); 

            data_arr[5 + BindAddr.Domain.Len] = (BindPort >> 8) & 0xff;
            data_arr[6 + BindAddr.Domain.Len] = BindPort & 0xff;
            
            data.assign(data_arr, data_arr + 6 + BindAddr.Domain.Len);
            break;
        case 0x04: // ipv6
            memmove(data_arr + 4, BindAddr.IPv6, 16);
            data_arr[20] = (BindPort >> 8) & 0xff;
            data_arr[21] = BindPort & 0xff;
            
            data.assign(data_arr, data_arr + 22);
            break;
        default: throw illegal_data_type();
        }
        return data;
    }
};
} // namespace lproxy::socks5
} // namespace lproxy

#endif // SOCKS5_PROTOCOL_H_1
