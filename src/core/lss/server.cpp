/*************************************************************************
	> File Name:    server.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 4:54:45
 ************************************************************************/
#include <thread>

#include <lss/config_server.h>
#include <lss/lss_server.h>
#include <lss/log.h>

int main(int argc, char* argv[]) 
try {
    // 加载配置文件
    lproxy::server::config::get_instance().configure("server-config.json");

    _print_s("[INFO] start log output thread...\n");
    // 启动日志输出线程
    std::thread thread_logoutput(lproxy::log::output_thread, 
            lproxy::server::config::get_instance().get_logfilename());
    thread_logoutput.detach();

    // 获取 本地 监听端口
    auto& bind_addr
        = lproxy::server::config::get_instance().get_bind_addr();
    uint16_t bind_port 
        = lproxy::server::config::get_instance().get_bind_port(); 

    // 启动 lss_server
    boost::asio::io_service io_service;

    //using namespace std; // for atoi
    lproxy::server::lss_server s(io_service, bind_addr, bind_port);
    io_service.run();

    _print_s("[INFO] Exit\n");
    return 0;
}
catch (const std::exception& e) {
    _print_s_err("[FATAL] Exception: " << e.what() 
            << " " << lproxy::log::basename(__FILE__) << std::endl);
    exit(1);
}
catch (...) {
    _print_s_err("[FATAL] An error has occurred" 
            << " " << lproxy::log::basename(__FILE__) << std::endl);
    exit(1);
} 

