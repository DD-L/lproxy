/*************************************************************************
	> File Name:    local.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 4:46:47
 ************************************************************************/
#include <iostream> // for std::cerr
#include <lss/config.h>
#include <lss/lss_server.h>
int main(int argc, char* argv[]) 
try {
    // step 0
    // 获取配置
    //if (argc != 2) {
    //    std::cerr << "Usage: " << argv[0] << " <port>\n";
    //    return 1;
    //}
    
    uint16_t local_bind_port 
        = lproxy::local::config::get_instance().get_local_bind_port(); 

    // step 1
    // 启动lss_server
    boost::asio::io_service io_service;

    //using namespace std; // for atoi
    lproxy::local::lss_server s(io_service, local_bind_port);
    io_service.run();

    return 0;
}
catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << std::endl;
}
catch (...) {
    std::cerr << "An error has occurred" << std::endl; 
}

