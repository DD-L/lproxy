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

#include <boost/variant.hpp>
#include <lss/typedefine.h>


namespace lproxy {

namespace socks5 {

// lproxy::socks5::_version_
static const byte _version_ = 0x05;

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

// 客户端 发来的 ID request
struct ident_req {
    byte Version = 0x05;
    byte NumberOfMethods;
    //byte Methods[256];
    data_t  Methods;
    ident_req(const byte* data_p, std::size_t len) {
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
}; // struct lproxy::socks5::ident_req

// 服务端 响应 ID response
struct ident_resp {
    /*
    byte Version = 0x05;
    byte Method;
    */
    static data_t& pack(data_t& data, ident_req* ident_req_p = nullptr) {
        byte Version = 0x05;
        byte Method  = 0x00;
        if (ident_req_p) {
            Version = ident_req_p->Version; // always 5
            // 从 ident_req_p->Methods 中选择一个
            for (auto& v : ident_req_p->Methods) {
                // 当前仅支持 0x00 的认证方法
                if (v == 0x00) {
                    byte data_arr[2] = {Version, Method}; 
                    data.assign(data_arr, data_arr + 2);
                    return data;
                }
            }
            // 没有一个认证方法被选中，客户端需要关闭连接
            byte data_arr[2] = {0x05, 0xff};
            data.assign(data_arr, data_arr + 2);
        }
        else {
            // 默认方法
            byte data_arr[2] = {Version, Method}; 
            data.assign(data_arr, data_arr + 2);
        }
        return data;
    } 
}; // struct lproxy::socks5::ident_resp

struct Domain_t {
    byte        Len;
    //std::string Name;
    data_t      Name;
};

// 应用于 boost::variant 最低要求是 支持拷贝构造 和 析构不能抛异常
template<std::size_t size>
struct Ip_t {
    typedef byte _ip_t[size];
    byte ip[size];
    Ip_t() {
        ::memset(ip, 0, size);
    }
    Ip_t(const Ip_t& that) {
        ::memmove(ip, that.ip, size);
    }
    //Ip_t(const byte ip_[size]) {
    Ip_t(const _ip_t ip_) {
        ::memmove(ip, ip_, size);
    }
    Ip_t& operator= (const Ip_t& that) {
        if (this != &that) {
            ::memmove(this->ip, that.ip, size);
        }
        return *this;
    }
    //Ip_t& operator= (const byte ip_[size]) {
    Ip_t& operator= (const _ip_t ip_) {
        *this = Ip_t(ip_);
    }
};

typedef Ip_t<4>  Ipv4_t;
typedef Ip_t<16> Ipv6_t;


// 客户端 发来的 request
struct req {
    byte Version = 0x05;
    //enum Version { V5 = 0x05 } version;
    byte Cmd;
    //enum Cmd { CONNECT = 0x01, BIND = 0x02, UDP = 0x03 } cmd;
    byte Reserved = 0x00;
    //enum Reserved { RESERVED = 0x00 } reserved;
    byte AddrType;
    //enum AddrType {IPv4 = 0x01, DOMAIN = 0x03, IPV6 = 0x04} addrType;

    boost::variant<Ipv4_t, Domain_t, Ipv6_t> DestAddr;
    /*
    union {
        byte IPv4[4];
        struct {
            byte Len;
            //byte name[256];
            std::string name; // not POD
        } Domain;
        byte IPv6[16];
    } DestAddr;
    */
    uint16_t DestPort;

    req(const byte* data_p, std::size_t len) {
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

        byte port_high_byte_ = 0, port_low_byte_ = 0;
        switch(AddrType) {
        case 0x01: // ipv4
            //memmove(DestAddr.IPv4, data_p + 4, 4);
            DestAddr = Ipv4_t(data_p + 4);
            
            port_high_byte_ = *(data_p + 8);
            port_low_byte_  = *(data_p + 9);
            
            break;
        case 0x03: {
            /*
            DestAddr.Domain.Len = *(data_p + 4);
            char name[DestAddr.Domain.Len];
            ::memmove(name, data_p + 5, DestAddr.Domain.Len);
            DestAddr.Domain.name.assign(name, name + DestAddr.Domain.Len);
            */
            const byte domain_len = *(data_p + 4);
            DestAddr = Domain_t({domain_len, data_t(data_p + 5, domain_len)});

            port_high_byte_ = *(data_p + 6 + domain_len);
            port_low_byte_  = *(data_p + 7 + domain_len);
            break;
        }
        case 0x04: 
            //memmove(DestAddr.IPv6, data_p + 4, 16);
            DestAddr = Ipv6_t(data_p + 4);

            port_high_byte_ = *(data_p + 20);
            port_low_byte_  = *(data_p + 21);
            break;
        default: throw illegal_data_type();
        }
        
        DestPort = (port_high_byte_ << 8) & 0xff00;
        DestPort = DestPort | port_low_byte_;
    }
}; // lproxy::socks5::req

// 服务端 响应 response
struct resp {
    byte Version = 0x05;
    byte Reply;
    byte Reserved = 0x00;
    byte AddrType;
    /*
    union {
        byte IPv4[4];
        struct {
            byte Len;
            std::string name; // not POD
        } Domain;
        byte IPv6[16];
    } BindAddr;
    */
    boost::variant<Ipv4_t, Domain_t, Ipv6_t> BindAddr;
    uint16_t  BindPort;

    // method
    void set_IPv4(const byte* ip, std::size_t size = 4) {
        assert(size == 4);
        AddrType = 0x01;
        //::memmove(BindAddr.IPv4, ip, 4);
        BindAddr = Ipv4_t(ip);
    }
    void set_IPv6(const byte* ip, std::size_t size = 16) {
        assert(size == 16);
        AddrType = 0x04;
        //::memmove(BindAddr.IPv4, ip, 16);
        BindAddr = Ipv6_t(ip);
    }
    //void set_Domain(const std::string& name) {
    void set_Domain(const data_t& name) {
        assert(name.size() < 256);
        AddrType = 0x03;
        /*
        BindAddr.Domain.Len = name.size();
        BindAddr.Domain.name = name;
        */
        BindAddr = Domain_t({(byte)name.size(), name});
    }
    void set_BindPort(const uint16_t port) {
        BindPort = port;
    }

    data_t& pack(data_t& data) {

        const std::size_t size = 4+1+256+2;
        byte data_arr[size] = {0}; 
        
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
            //memmove(data_arr + 4, BindAddr.IPv4, 4);
            memmove(data_arr + 4, boost::get<Ipv4_t>(BindAddr).ip, 4);
            data_arr[8] = (BindPort >> 8) & 0xff;
            data_arr[9] = BindPort & 0xff;

            data.assign(data_arr, data_arr + 10);
            break;
        case 0x03: // domain
            /*
            if (6 + BindAddr.Domain.Len >= size) {
                throw illegal_data_type();
            }

            data_arr[4] = BindAddr.Domain.Len;
            memmove(data_arr + 5, BindAddr.Domain.name.c_str(), 
                    BindAddr.Domain.Len); 

            data_arr[5 + BindAddr.Domain.Len] = (BindPort >> 8) & 0xff;
            data_arr[6 + BindAddr.Domain.Len] = BindPort & 0xff;
            
            data.assign(data_arr, data_arr + 6 + BindAddr.Domain.Len);
            */
            {
            auto& domain = boost::get<Domain_t>(BindAddr);
            if (std::size_t(6 + domain.Len) >= size) {
                throw illegal_data_type();
            }

            data_arr[4] = domain.Len;
            memmove(data_arr + 5, domain.Name.c_str(), domain.Len); 

            data_arr[5 + domain.Len] = (BindPort >> 8) & 0xff;
            data_arr[6 + domain.Len] = BindPort & 0xff;

            data.assign(data_arr, data_arr + 6 + domain.Len);
            }
            break;
        case 0x04: // ipv6
            //memmove(data_arr + 4, BindAddr.IPv6, 16);
            memmove(data_arr + 4, boost::get<Ipv6_t>(BindAddr).ip, 16);
            data_arr[20] = (BindPort >> 8) & 0xff;
            data_arr[21] = BindPort & 0xff;
            
            data.assign(data_arr, data_arr + 22);
            break;
        default: throw illegal_data_type();
        }
        return data;
    }
}; // struct lproxy::socks5:resp
} // namespace lproxy::socks5
} // namespace lproxy

#endif // SOCKS5_PROTOCOL_H_1
