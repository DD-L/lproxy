/*************************************************************************
	> File Name:    client.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 4:46:47
 ************************************************************************/

#include "lss/lss_server.h"
int main(int argc, char* argv[]) 
try {
    // step 0
    // 获取配置
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return 1;
    }

    // step 1
    // 启动lss_server
    boost::asio::io_service io_service;

    using namespace std; // for atoi
    lproxy::local::lss_server s(io_service, atoi[argv[1]]);
    io_service.run();

    return 0;

catch (const std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
}

