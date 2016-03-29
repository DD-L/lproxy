/*************************************************************************
	> File Name:    local.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 4:46:47
 ************************************************************************/
#include <lss/config_local.h>
#include <lss/lss_server.h>
#include <thread>
#include <lss/log.h>
#include <program_options/program_options.h>

static void process_program_options(const program_options& po,
        std::string& local_config_file) {
    if (po.empty()) {
        local_config_file = "local-config.json";
        return;
    }
    if (po.count("-h") || po.count("--help")) {
        std::string message = po.show_help(
                "\"local\" side of \"lproxy service\"");
        _print_s(message);
        exit(0);
    }
    try {
        if (po.count("-c")) {
            std::string value = po.get("-c");
            local_config_file = value;
            return;
        }
        if (po.count("--config")) {
            std::string value = po.get("--config");
            local_config_file = value;
            return;
        }
    }
    catch (const program_options::parameter_error& error) {
        _print_s_err("[FATAL] " << error.what() << std::endl);
        exit(-1);
    }

    _print_s_err("[FATAL] Unsupported options" << std::endl); 
    exit(1);
}

int main(int argc, char* argv[]) 
try {
    // 参数处理
    program_options po("lsslocal.exe [option]");

    po.add_option("-h, --help", "Show this message.");
    //po.add_option("-v, --version", "Show current version.");
    po.add_option("-c, --config", "Specify which configuration file "
            "lsslocal.exe should\nuse instead of the default.\n"
            "If not specified, the default configuration file is\n"
            "'local-config.json' in the current working directory");

    po.example("lsslocal.exe");
    po.example("lsslocal.exe -c /path/to/local-config.json");

    po.store(argc, argv);

    std::string local_config_file;
    process_program_options(po, local_config_file);
    _print_s("[INFO] configuration file: " << local_config_file << std::endl);

    // 加载配置文件
    lproxy::local::config::get_instance().configure(local_config_file);

    _print_s("[INFO] start log output thread...\n");
    // 启动日志输出线程
    std::thread thread_logoutput(lproxy::log::output_thread, 
            lproxy::local::config::get_instance().get_logfilename());
    thread_logoutput.detach();


    // 获取 本地 监听端口
    auto& bind_addr
        = lproxy::local::config::get_instance().get_bind_addr();
    uint16_t bind_port 
        = lproxy::local::config::get_instance().get_bind_port(); 

    // 启动 lss_server
    boost::asio::io_service io_service;

    //using namespace std; // for atoi
    lproxy::local::lss_server s(io_service, bind_addr, bind_port);
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

