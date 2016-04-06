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

#include <program_options/program_options.h>
#include <cstdlib> // for ::system() exit()

static void process_program_options(const program_options& po,
        std::string& server_config_file) {
    if (po.empty()) {
        server_config_file = "server-config.json";
        return;
    }
    if (po.count("-h") || po.count("--help")) {
        std::string message = po.show_help(
                "\"server\" side of \"lproxy service\"");
        _print_s(message);
        exit(0);
    }
    if (po.count("-k") || po.count("--keep-running")) {
        auto&& argv = po.get_argv();
        std::string keep_exe;
        const bool ret = lproxy::get_keep_exe_path(keep_exe, argv[0]);
        if (ret == false) {
            // keep_exe 不存在
            _print_s_err("[FATAL] " << keep_exe << " not exists! "
                    "(Specified parameter \"-k\" or \"--keep-running\")" 
                    << std::endl);
            exit(-1); 
        }
        // 组装 lkeep.exe 的运行参数
        std::string cmd_line = keep_exe + " --run " 
            // 修饰 argv[0]. 如果 argv[0] 路径有空格, 添加引号
            + lproxy::adorn_appname(argv[0]);

        for (auto&& cit = argv.cbegin() + 1; cit != argv.cend(); ++cit) {
            if (*cit == "-k" || *cit == "--keep-running") continue;
            cmd_line += (" " + *cit);
        }
        _print_s("[info] start a new process: " << cmd_line << std::endl);
        // TODO
        // https://github.com/BorisSchaeling/boost-process
        // http://www.highscore.de/boost/process0.5/
        // 以后要用 boost.process 替换 简陋粗糙的 ::system()
        exit(::system(cmd_line.c_str())); // ::system 的返回值很讨厌
    }
    try {
        if (po.count("-c")) {
            std::string value = po.get("-c");
            server_config_file = value;
            return;
        }
        if (po.count("--config")) {
            std::string value = po.get("--config");
            server_config_file = value;
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

void option(int argc, char** argv, std::string& server_config_file) {
    assert(argv);
    // 参数处理
    program_options po("lssserver.exe [option]");

    po.add_option("-h, --help", "Show this message.");
    //po.add_option("-v, --version", "Show current version.");
    po.add_option("-c, --config <profile>", 
            "Specify which configuration file lssserver.exe should\n"
            "use instead of the default.\n"
            "If not specified, the default configuration file is\n"
            "'server-config.json' in the current working directory.");
    po.add_option("-k, --keep-running", 
            "Keep this program running. Restart immediately after\n"
            "the program quit unexpectedly.");

    po.example("lssserver.exe");
    po.example("lssserver.exe -c /path/to/server-config.json");
    po.example("lssserver.exe -c /path/to/server-config.json -k");

    po.store(argc, argv);
    process_program_options(po, server_config_file);
}

int main(int argc, char* argv[]) 
try {
    // get server_config_file
    std::string server_config_file;
    option(argc, argv, server_config_file);
    _print_s("[INFO] configuration file: " << server_config_file << std::endl);

    // 加载配置文件
    lproxy::server::config::get_instance().configure(server_config_file);

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
            << " " << lproxy::log::basename(__FILE__) << ":" 
            << __LINE__ << std::endl);
    exit(1);
}
catch (...) {
    _print_s_err("[FATAL] An error has occurred" 
            << " " << lproxy::log::basename(__FILE__) << ":" 
            << __LINE__ << std::endl);
    exit(1);
} 

