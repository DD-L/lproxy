#ifndef LSS_TYPEDEFINE_H_1
#define LSS_TYPEDEFINE_H_1
/*************************************************************************
	> File Name:    typedefine.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/03/05 - 07:00:52
 ************************************************************************/

#include <stdint.h>
#include <string>
#include <vector>
#include <set>

namespace lproxy {

typedef uint8_t                 byte;
typedef uint16_t                data_len_t;
typedef uint16_t                keysize_t;

// lproxy::data_t
typedef std::basic_string<byte> data_t;
typedef std::string             sdata_t;
typedef std::vector<byte>       vdata_t;

} // namespace lproxy

#endif // LSS_TYPEDEFINE_H_1

