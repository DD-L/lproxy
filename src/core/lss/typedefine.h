#ifndef LSS_TYPEDEFINE_H_1
#define LSS_TYPEDEFINE_H_1
/*************************************************************************
	> File Name:    typedefine.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/03/05 - 07:00:52
 ************************************************************************/

// TODO
//#define LSS_DEBUG

#include <lss/macros.h>
#include <iostream>
#include <stdint.h>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <type_traits> 
#include <memory>

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <boost/filesystem.hpp>

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
static inline shared_data_type make_shared_data(const std::size_t data_length,
        data_t::value_type v) {
    return std::make_shared<data_t>(data_length, v);
}
/**
 * @brief make_shared_data
 * @param data 默认值为 data_t()
 * @return     返回一个shared_data_type 类型临时对象, 此对象指向一个 data 的副本
 */
template<typename DATATYPE>
static inline shared_data_type make_shared_data(DATATYPE&& data = data_t()) {
    return std::make_shared<DATATYPE>(std::forward<DATATYPE>(data));
}
static inline shared_data_type make_shared_data(void) {
    return std::make_shared<data_t>();
}



/**
 * @brief _debug_format_data 格式化一组数据
 * @param data        要打印的容器
 * //@param data_length 能打印容器的最大长度
 * @param VTYPE       容器元素打印的强制转换类型
 * @param c           容器元素之间输出间隔符; 默认值char(' '), c = 0 时无间隔符
 * @param f           流输出控制函数; 默认值 std::dec
 * @return 返回一个 sdata_t 的临时对象
 */
template <typename DATA_TYPE, typename VTYPE, typename F = decltype(std::dec)>
//std::ostringstream& _debug_format_data(const DATA_TYPE& data, VTYPE, 
sdata_t _debug_format_data(const DATA_TYPE& data, VTYPE, 
        char c = ' ', F f = std::dec) {
#ifdef LSS_DEBUG
    std::ostringstream oss;
    //oss.flush();
    for (auto& v : data) {
        oss << f << VTYPE(v) << c;
    }
    return oss.str();
#endif
    return "";
}


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

/**
 * @brief adorn_appname 修饰 appname 
 *                      给 appname 路径添加引号, 去除 appname 路径有空格的影响
 * @param appname
 * @return 返回修饰后的结果, 一个 sdata_t 的临时对象
 */
static inline sdata_t adorn_appname(const boost::filesystem::path& appname) {
    return sdata_t("\"" + appname.string() + "\"");
}
static inline sdata_t adorn_appname(boost::filesystem::path&& appname) {
    return adorn_appname(appname);
}
static inline sdata_t adorn_appname(const sdata_t& appname) {
    namespace fs = boost::filesystem;
    return adorn_appname(fs::path(appname.begin(), appname.end()));
}

/**
 * @brief  get_keep_exe_path. 获取 keep_exe
 * @param  keep_exe.          类型: sdata_t&       [out]
 * @param  argv0.             类型: const sdata_t& [in]  主函数参数argv[0]
 * @return 成功返回 true, 失败或 keep_exe 文件不存在返回 false.
*/
static inline bool get_keep_exe_path(sdata_t& keep_exe, const sdata_t& argv0) {
    const sdata_t _keep_exe = "lkeep.exe";
    namespace fs = boost::filesystem;

    // 修饰 keep_exe
    fs::path this_app(argv0.begin(), argv0.end());
    fs::path this_app_path = this_app.remove_filename();
    fs::path keep_exe_path;
    if (this_app_path.string() == "") {
        // 处理: 不带路径信息的运行了程序, 比如在
        // 1. windows 下直接运行 > app.exe
        // 2. linux 下 将 . 配置到 PATH 中
        //      $ export PATH=$PATH:.
        //      $ app.exe
        keep_exe_path.assign(_keep_exe.begin(), _keep_exe.end());
    }
    else {
        keep_exe_path = this_app_path / "/" / _keep_exe;
    }

    // 给路径加引号
    keep_exe = adorn_appname(keep_exe_path);

    // 检测 keep_exe 是否存在.
    return fs::exists(keep_exe_path);
}


/////////////////////////////////////////////////////////////////////////
// placeholder
/////////////////////////////////////////////////////////////////////////

// namespace lproxy::placeholders
namespace placeholders {

// lproxy::placeholders::shared_data
static auto&& shared_data = make_shared_data();

} // namespace lproxy::placeholders

// lproxy::readbuffer
namespace readbuffer {
    // lproxy::readbuffer::max_length
    //enum             { max_length = 1024 };
    enum             { max_length = 4096, length_handshake = 1024 };
} // namespace lproxy::readbuffer


} // namespace lproxy

#endif // LSS_TYPEDEFINE_H_1

