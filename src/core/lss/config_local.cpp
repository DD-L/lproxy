/*************************************************************************
	> File Name:    config_local.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/27 14:56:58
 ************************************************************************/


#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
using namespace boost::property_tree;

#include <crypto/md5_crypto.h>
#include <lss/config_local.h>
#include <lss/log.h>

void lproxy::local::config::configure(const sdata_t& config_file)
try {
    ptree pt;
    read_json(config_file, pt);

    m_bind_addr   = pt.get<sdata_t>("lsslocal.bind_addr");
    m_bind_port   = pt.get<uint16_t>("lsslocal.bind_port");
    m_server_name = pt.get<sdata_t>("lsslocal.server_name");
    m_server_port = pt.get<sdata_t>("lsslocal.server_port");
    m_auth_key    = pt.get<sdata_t>("lsslocal.auth_key");
    m_zip_on      = pt.get<bool>("lsslocal.zip_on", false);
    m_logfile     = pt.get<sdata_t>("lsslocal.logfile", "");
    m_timeout     = pt.get<uint32_t>("lsslocal.timeout", 30);

    _print_s("[INFO] bind_addr:   " << m_bind_addr   << std::endl);
    _print_s("[INFO] bind_port:   " << m_bind_port   << std::endl);
    _print_s("[INFO] server_name: " << m_server_name << std::endl);
    _print_s("[INFO] server_port: " << m_server_port << std::endl);
#ifdef LSS_DEBUG
    _print_s("[DEBUG]auth_key:    " << m_auth_key    << std::endl);
#else
    _print_s("[INFO] auth_key:    ***"               << std::endl);
#endif
    _print_s("[INFO] zip_on:      " << std::boolalpha << m_zip_on << std::endl);
    _print_s("[INFO] logfile:     " << m_logfile     << std::endl);
    _print_s("[INFO] timeout:     " << m_timeout     << std::endl);

    // gen cipher m_auth_key
    crypto::Encryptor md5(new crypto::Md5());
    sdata_t cipher_key;
    md5.encrypt(cipher_key, &m_auth_key[0], m_auth_key.size());
    m_auth_key = cipher_key;
}
catch (const ptree_error& e) {
    _print_s_err("[FATAL] " << e.what() 
            << ". An error occurred when reading the configuration file '" 
            << config_file << "'" << std::endl);
    exit(2);
}
catch (std::exception& e) {
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


