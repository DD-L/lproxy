#ifndef LSS_TYPEDEFINE_H_1
#define LSS_TYPEDEFINE_H_1
/*************************************************************************
	> File Name:    typedefine.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/03/05 - 07:00:52
 ************************************************************************/
#include <lss/macros.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <set>
#include <type_traits> 

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

namespace lproxy {

typedef uint8_t                 byte;
typedef uint16_t                data_len_t;
typedef uint16_t                keysize_t;

typedef byte*                   byte_ptr;
typedef const byte*             const_byte_ptr;

// lproxy::data_t
typedef std::basic_string<byte> data_t;
typedef std::string             sdata_t;
typedef std::vector<byte>       vdata_t;

// boost
using namespace boost::asio;
using ip::tcp;
using ip::udp;

// function


} // namespace lproxy

#endif // LSS_TYPEDEFINE_H_1

