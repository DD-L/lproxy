/*************************************************************************
	> File Name:    local.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 4:46:47
 ************************************************************************/
#include <iostream> // for std::cerr
#include <lss/config.h>
#include <lss/lss_server.h>
#include <thread>
#include <lss/log.h>
int main(int argc, char* argv[]) 
try {
    _print_s("start log output thread...\n");
    // 启动日志输出线程
    std::thread thread_logoutput(lproxy::log::output_thread, 
            lproxy::log::LOCAL);
    (void)thread_logoutput;

// test
    loginfo("this is a test...");

    // step 0
    // 获取配置
    uint16_t local_bind_port 
        = lproxy::local::config::get_instance().get_local_bind_port(); 

    // step 1
    // 启动lss_server
    boost::asio::io_service io_service;

    //using namespace std; // for atoi
    lproxy::local::lss_server s(io_service, local_bind_port);
    io_service.run();

    _print_s("Exit");
    return 0;
}
catch (const std::exception& e) {
    //std::cerr << "Exception: " << e.what() << std::endl;
    _print_s_err("Exception: " << e.what() << std::endl);
}
catch (...) {
    //std::cerr << "An error has occurred" << std::endl; 
    _print_s_err("An error has occurred" << std::endl);
}

