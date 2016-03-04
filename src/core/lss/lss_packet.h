#ifndef _LSS_PACKET_H_1
#define _LSS_PACKET_H_1
/*************************************************************************
	> File Name:    lss_packet.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/4 10:50:30
 ************************************************************************/

#include <stdint.h>
//#include <vector>
#include <string>

namespace lproxy {

typedef uint8_t           byte;
typedef uint16_t          data_len_t;
//typedef std::vector<byte> data_t;
typedef std::string       data_t;

struct __packet {
public:
    byte   version;
    byte   type;
    byte   data_len_high_byte,  data_len_low_byte;
    data_t data;
private:
    typedef __packet THIS_CLASS;
public:
    THIS_CLASS(void) : version(0xff), type(0xff), 
                       data_len_high_byte(0x00), data_len_low_byte(0x00),
                       data() {}
    
    THIS_CLASS(byte version_,  byte pack_type_, 
               data_len_t data_len_, const data_t& data_)

                : version(version_), type(pack_type_), 
                  data_len_high_byte((data_len_ >> 8) & 0xff),
                  data_len_low_byte(data_len_ & 0xff), data(data_) {}

    THIS_CLASS(byte version_, byte pack_type_,  byte data_len_high_, 
               byte data_len_low_, const data_t& data_)

                : version(version_), type(pack_type_), 
                  data_len_high_byte(data_len_high_),
                  data_len_low_byte(data_len_high_), data(data_) {}

    THIS_CLASS(byte version_,  byte pack_type_, 
               data_len_t data_len_, data_t&& data_)

                : version(version_), type(pack_type_), 
                  data_len_high_byte((data_len_ >> 8) & 0xff),
                  data_len_low_byte(data_len_ & 0xff), 
                  data(std::move(data_)) {}

    THIS_CLASS(byte version_, byte pack_type_,  byte data_len_high_, 
               byte data_len_low_, data_t&& data_)

                : version(version_), type(pack_type_), 
                  data_len_high_byte(data_len_high_),
                  data_len_low_byte(data_len_high_), data(std::move(data_)) {}

    virtual ~THIS_CLASS(void) {}

    
}; // struct lproxy::__packet

struct __request_type {
    enum PackType {
        hello    = 0x00,
        exchange = 0x02,
        data     = 0x06,
        zipdata  = 0x17,
        bad      = 0xff
    };
}; // struct lproxy::__request_type

struct __reply_type {
    enum PackTpye {
        hello    = 0x01,
        exchange = 0x03,
        deny     = 0x04,
        timeout  = 0x05,
        data     = 0x06,
        zipdata  = 0x17,
        bad      = 0xff
    };
}; // struct lproxy::__reply_type


namespace local {

class request : public __request_type {
typedef request THIS_CLASS;
public:
    THIS_CLASS(byte version,        byte pack_type, 
               data_len_t data_len, const data_t& data)
        : pack(version, pack_type, data_len, data) {}

    THIS_CLASS(byte version, byte pack_type,  byte data_len_high, 
               byte data_len_low, const data_t& data)
        : pack(version, pack_type, data_len_high, data_len_low, data) {}

    THIS_CLASS(byte version,        byte pack_type, 
               data_len_t data_len, data_t&& data)
        : pack(version, pack_type, data_len, std::move(data)) {}

    THIS_CLASS(byte version, byte pack_type,  byte data_len_high, 
               byte data_len_low, data_t&& data)
        : pack(version, pack_type, data_len_high, 
                data_len_low, std::move(data)) {}

    virtual ~THIS_CLASS(void) {}


    boost::array<boost::asio::const_buffer, 5> buffers(void) const {
        boost::array<boost::asio::const_buffer, 5> bufs = 
        {
            {
                boost::asio::buffer(&(pack.version), 1), 
                boost::asio::buffer(&(pack.type),    1), 
                boost::asio::buffer(&(pack.data_len_high_byte),  1), 
                boost::asio::buffer(&(pack.data_len_low_byte),   1), 
                boost::asio::buffer(pack.data), 
            }
        };
        return bufs;
    }

private:
    __packet pack;
}; // class lproxy::local::request

class reply : public __reply_type {
typedef request THIS_CLASS;
public:
    THIS_CLASS(void) : pack() {}
    virtual ~THIS_CLASS(void) {}

    boost::array<boost::asio::mutable_buffer, 5> buffers() {
        boost::array<boost::asio::mutable_buffer, 5> bufs =
        {
            {
                boost::asio::buffer(&(pack.version), 1), 
                boost::asio::buffer(&(pack.type),  1), 
                boost::asio::buffer(&(pack.data_len_high_byte),  1), 
                boost::asio::buffer(&(pack.data_len_low_byte),   1), 
                boost::asio::buffer(pack.data), 
                
            }
        };
        return bufs;
    }

    byte version(void) {
        return pack.version;
    }

    reply::PackTpye type(void) {
        return pack.type;
    }
    
    data_len_t data_len(void) {
        data_len_t len = pack.data_len_high_byte;
        return ((len << 8) & 0xff00) | pack.data_len_low_byte;
    }

    data_t& data(void) {
        return pack.data;
    }
    
private:
    __packet pack;
}; // class lproxy::local::reply


} // namespace lproxy::local

namespace server {

class request : __request_type {
typedef request THIS_CLASS;
public:
    THIS_CLASS(void) : pack() {}
    virtual ~THIS_CLASS(void) {}

    boost::array<boost::asio::mutable_buffer, 5> buffers() {
        boost::array<boost::asio::mutable_buffer, 5> bufs =
        {
            {
                boost::asio::buffer(&(pack.version), 1), 
                boost::asio::buffer(&(pack.type),  1), 
                boost::asio::buffer(&(pack.data_len_high_byte),  1), 
                boost::asio::buffer(&(pack.data_len_low_byte),   1), 
                boost::asio::buffer(pack.data), 
            }
        };
        return bufs;
    }
private:
    __packet pack;
}; // class lproxy::server::request

class reply : __reply_type {
typedef request THIS_CLASS;
public:
    THIS_CLASS(byte version,        byte pack_type, 
               data_len_t data_len, const data_t& data)
        : pack(version, pack_type, data_len, data) {}

    THIS_CLASS(byte version, byte pack_type,  byte data_len_high, 
               byte data_len_low, const data_t& data)
        : pack(version, pack_type, data_len_high, data_len_low, data) {}

    THIS_CLASS(byte version,        byte pack_type, 
               data_len_t data_len, data_t&& data)
        : pack(version, pack_type, data_len, std::move(data)) {}

    THIS_CLASS(byte version, byte pack_type,  byte data_len_high, 
               byte data_len_low, data_t&& data)
        : pack(version, pack_type, data_len_high, 
                data_len_low, std::move(data)) {}

    virtual ~THIS_CLASS(void) {}


    boost::array<boost::asio::const_buffer, 5> buffers(void) const {
        boost::array<boost::asio::const_buffer, 5> bufs = 
        {
            {
                boost::asio::buffer(&(pack.version), 1), 
                boost::asio::buffer(&(pack.type),    1), 
                boost::asio::buffer(&(pack.data_len_high_byte),  1), 
                boost::asio::buffer(&(pack.data_len_low_byte),   1), 
                boost::asio::buffer(pack.data), 
            }
        };
        return bufs;
    }

private:
    __packet pack;
}; // class lproxy::server::reply

} // namespace lproxy::server

} // namespace lproxy

#endif // _LSS_PACKET_H_1
