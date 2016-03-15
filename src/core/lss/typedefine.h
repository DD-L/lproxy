#ifndef LSS_TYPEDEFINE_H_1
#define LSS_TYPEDEFINE_H_1
/*************************************************************************
	> File Name:    typedefine.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/03/05 - 07:00:52
 ************************************************************************/

// TODO
#define LSS_DEBUG

#include <lss/macros.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <vector>
#include <set>
#include <type_traits> 
#include <memory>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

namespace lproxy {

/////////////////////////////////////////////////////////////////////////
// type
/////////////////////////////////////////////////////////////////////////

typedef uint8_t                  byte;
typedef uint16_t                 data_len_t;
typedef uint16_t                 keysize_t;

typedef byte*                    byte_ptr;
typedef const byte*              const_byte_ptr;

// lproxy::data_t
typedef std::basic_string<byte>  data_t;
typedef std::string              sdata_t;
typedef std::vector<byte>        vdata_t;

// lproxy::shared_data_type
typedef std::shared_ptr<data_t>  shared_data_type;
/*
typedef std::shared_ptr<sdata_t> shared_sdata_type;
typedef std::shared_ptr<vdata_t> shared_vdata_type;
*/

// boost
using namespace boost::asio;
using ip::tcp;
using ip::udp;


/////////////////////////////////////////////////////////////////////////
// function
/////////////////////////////////////////////////////////////////////////

/**
 * @brief make_shared_data 
 * @param data_length
 * @param v
 * @return              返回一个容器长度为 data_length, 且被 v 
 *                      填充的 shared_data_type 类型临时对象 
 */
inline shared_data_type make_shared_data(const std::size_t data_length,
        data_t::value_type v) {
    return std::make_shared<data_t>(data_length, v);
}
/**
 * @brief make_shared_data
 * @param data 默认值为 data_t()
 * @return     返回一个shared_data_type 类型临时对象, 此对象指向一个 data 的副本
 */
template<typename DATATYPE>
inline shared_data_type make_shared_data(DATATYPE&& data = data_t()) {
    return std::make_shared<DATATYPE>(std::forward<DATATYPE>(data));
}
inline shared_data_type make_shared_data(void) {
    return std::make_shared<data_t>();
}



/**
 * @brief _debug_print_data 打印一组数据
 * @param data        要打印的容器
 * //@param data_length 能打印容器的最大长度
 * @param VTYPE       容器元素打印的强制转换类型
 * @param c           容器元素之间输出间隔符; 默认值char(' '), c = 0 时无间隔符
 * @param f           流输出控制函数; 默认值 std::dec
 * @return void
 */
template <typename DATA_TYPE, typename VTYPE, typename F = decltype(std::dec)>
void _debug_print_data(const DATA_TYPE& data, VTYPE, 
        char c = ' ', F f = std::dec) {
#ifdef LSS_DEBUG
    for (auto& v : data) {
        std::cout << f << VTYPE(v) << c;
    }
    std::cout << std::dec << std::endl;
#endif
}

/*
template <typename DATA_TYPE, typename VTYPE, typename F = decltype(std::dec)>
void _debug_print_data(const DATA_TYPE& data, const std::size_t data_length, 
        VTYPE, char c = ' ', F f = std::dec) {
#ifdef LSS_DEBUG
    for (auto i = data.cbegin(); i != data.cbegin() + data_length; ++i) {
        if (i == data.cend()) break;
        std::cout << f << VTYPE(*i) << c;
    }
    std::cout << std::dec << std::endl;
#endif
}
*/

/**
 * @brief get_vdata_from_lss_pack: 将 'lss_pack' 里的数据 转换为 vdata_t 类型 
 * @param lss:  'lss_pack'
 *      'lss_pack' 可以是下列类型的对象:
 *      lproxy::local::reply, lproxy::local::request, 
 *      lproxy::server::reply, lproxy::server::request
 * @return 返回一个 vdata_t 类型的临时对象
 */
template <typename T>
vdata_t get_vdata_from_lss_pack(T&& lss) {
    vdata_t _test(boost::asio::buffer_size(std::forward<T>(lss).buffers()));
    boost::asio::buffer_copy(boost::asio::buffer(_test), 
            std::forward<T>(lss).buffers());
    return _test;
}


/////////////////////////////////////////////////////////////////////////
// placeholder
/////////////////////////////////////////////////////////////////////////

// namespace lproxy::placeholders
namespace placeholders {

// lproxy::placeholders::shared_data
static auto&& shared_data = make_shared_data();

} // namespace lproxy::placeholders


} // namespace lproxy

#endif // LSS_TYPEDEFINE_H_1

