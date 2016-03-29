/*************************************************************************
	> File Name:    config_server.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/28 13:43:53
 ************************************************************************/

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;

#include <lss/config_server.h>
#include <lss/log.h>

// ptree 很容易搞定这样的json数组(元素是键值对): {"a" : [{"1": 1}, {"2": 2}]}
// 但像这样的数组: {"a": [1, 2, 3] }, 就需要点技巧了:
//http://stackoverflow.com/questions/23481262/using-boost-property-tree-to-read-int-array
//  { "a": [8, 6, 2], "b": [2, 2, 1] }
//The property tree dataset is not typed, and does not support arrays as such. 
// Thus, the following JSON / property tree mapping is used:
//* JSON objects are mapped to nodes. Each property is a child node.
//* JSON arrays are mapped to nodes. Each element is a child node with an empty
//      name. If a node has both named and unnamed child nodes, it cannot be 
//      mapped to a JSON representation.
//* JSON values are mapped to nodes containing the value. However, all type 
//      information is lost; numbers, as well as the literals "null", "true" 
//      and "false" are simply mapped to their string form.
//* Property tree nodes containing both child nodes and data cannot be mapped.
//You can iterate the array with a helper function:
//
// for (auto& v : as_vector<int>(pt, "a")) {
//      cout << v << endl; 
// }
#include <vector>
template <typename T>
static std::vector<T> as_vector(ptree const& pt, ptree::key_type const& key)
{
    std::vector<T> r;
    for (auto& item : pt.get_child(key)) {
        assert(item.first.empty()); // array elements have no names
        r.push_back(item.second.get_value<T>());
    }
    return r;
}
// 也可以这样访问 a (但如果还要顾及 v.second.data() 的类型信息的话, 会有些麻烦):
// for(boost::property_tree::ptree::value_type &v : pt.get_child("a.")) {
//    cout << v.second.data() << endl;
// }


void lproxy::server::config::configure(const sdata_t& config_file)
try {
    ptree pt;
    read_json(config_file, pt);
    
    m_bind_addr = pt.get<sdata_t>("lssserver.bind_addr");
    m_bind_port = pt.get<uint16_t>("lssserver.bind_port");
    m_bind_addr_socks5     = pt.get<sdata_t>("lssserver.bind_addr_socks5");
    m_bind_addr_type_socks5= pt.get<sdata_t>("lssserver.bind_addr_type_socks5");
    m_bind_port_socks5     = pt.get<uint16_t>("lssserver.bind_port_socks5");
    m_zip_on    = pt.get<bool>("lssserver.zip_on", false);
    m_timeout   = pt.get<uint32_t>("lssserver.timeout", 30);
    m_logfile   = pt.get<sdata_t>("lsslocal.logfile", "");

    for (auto& cipher_auth_key 
            : ::as_vector<sdata_t>(pt, "lssserver.cipher_auth_key_set")) {
        assert(cipher_auth_key.length() == 32);
        auto ret = m_cipher_auth_key_set.insert(
                (const_byte_ptr)cipher_auth_key.c_str());
        if (ret.second == false) {
            _print_s("[WARN] redundant auth_key: " << cipher_auth_key 
                    << std::endl);
        }
    }

    _print_s("[INFO] bind_addr:             " << m_bind_addr   << std::endl);
    _print_s("[INFO] bind_port:             " << m_bind_port   << std::endl);
    _print_s("[INFO] bind_addr_socks5:      " << m_bind_addr_socks5 
            << std::endl);
    _print_s("[INFO] bind_addr_type_socks5: " << m_bind_addr_type_socks5 
            << std::endl);
    _print_s("[INFO] bind_port_socks5:      " << m_bind_port_socks5 
            << std::endl);
    _print_s("[INFO] zip_on:                " << std::boolalpha << m_zip_on 
            << std::endl);
    _print_s("[INFO] logfile:               " << m_logfile     << std::endl);
    _print_s("[INFO] timeout:               " << m_timeout     << std::endl);

#ifdef LSS_DEBUG
    _print_s("[DEBUG]cipher_auth_keys:      ");
#else
    _print_s("[INFO] cipher_auth_keys:      ");
#endif

    for (auto v : m_cipher_auth_key_set) {
#ifdef LSS_DEBUG
        _print_s(v.c_str() << ", ");
#else
        _print_s("***, ");
#endif
    }
    _print_s(std::endl);
}
catch (const ptree_error& e) {
    _print_s_err("[FATAL] " << e.what() 
            << ". An error occurred when reading the configuration file '" 
            << config_file << "'" << std::endl);
    exit(2);
}
catch (...) {
    _print_s_err("[FATAL] An error occurred when reading the configuration "
            "file '" << config_file << "'" << std::endl);
    exit(2);
}
