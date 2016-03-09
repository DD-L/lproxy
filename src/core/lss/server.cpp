/*************************************************************************
	> File Name:    server.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 4:54:45
 ************************************************************************/

#include "lss/lss_server.h"
int main(int argc, char* argv[]) 
try {
    // step 0
    // 获取配置
    uint16_t bind_port 
        = lproxy::server::config::get_instance().get_bind_port(); 

    // step 1
    // 启动lss_server
    boost::asio::io_service io_service;

    //using namespace std; // for atoi
    lproxy::server::lss_server s(io_service, bind_port);
    io_service.run();

    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
}
catch (...) {
    std::cerr << "An error has occurred" << std::endl; 
} 

