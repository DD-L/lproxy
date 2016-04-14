/*************************************************************************
	> File Name:    keep.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/31 8:46:24
 ************************************************************************/

#include <iostream>
#include <string>
#include <program_options/program_options.h>
#include <cstdlib> // for ::system() exit()
#include <thread>
#include <chrono>
#include <boost/lexical_cast.hpp>

struct arg_t {
    std::string command_line;
    int interval   = -1;
    int max_cycles = -1;
};

static inline void process_program_options(const program_options& po, 
        arg_t& arg) {
    if (po.empty() || po.count("-h") || po.count("--help")) {
        std::string message = po.show_help("Execute a command in a loop.");
        std::cout << message << std::endl;
        exit(0);
    }

    try {
        if (po.count("-i")) {
            try {
                arg.interval = boost::lexical_cast<int>(po.get("-i"));
                if (arg.interval < 0) {
                    throw boost::bad_lexical_cast();
                }
            }
            catch (boost::bad_lexical_cast& e) {
                std::cerr << "[FATAL] bad number. Specified parameter "
                    "\"-i\"." << std::endl;
                exit(-1);
            }
        }
        else if (po.count("--interval")) {
            try {
                arg.interval = boost::lexical_cast<int>(po.get("--interval"));
                if (arg.interval < 0) {
                    throw boost::bad_lexical_cast();
                }
            }
            catch (boost::bad_lexical_cast& e) {
                std::cerr << "[FATAL] bad number. Specified parameter "
                    "\"--interval\"." << std::endl;
                exit(-1);
            }
        }

        if (po.count("-m")) {
            try {
                arg.max_cycles = boost::lexical_cast<int>(po.get("-m"));
                if (arg.max_cycles < 0) {
                    throw boost::bad_lexical_cast();
                }
            }
            catch (boost::bad_lexical_cast& e) {
                std::cerr << "[FATAL] bad number. Specified parameter "
                    "\"-m\"." << std::endl;
                exit(-1);
            }
        }
        else if (po.count("--max")) {
            try {
                arg.max_cycles = boost::lexical_cast<int>(po.get("--max"));
                if (arg.max_cycles < 0) {
                    throw boost::bad_lexical_cast();
                }
            }
            catch (boost::bad_lexical_cast& e) {
                std::cerr << "[FATAL] bad number. Specified parameter "
                    "\"--max\"." << std::endl;
                exit(-1);
            }
        }

        if (po.count("-r") || po.count("--run")) {
            auto&& argv = po.get_argv();
            bool pos_flag = false;
            for (auto& v : argv) {
                if (pos_flag) {
                    arg.command_line += (v + " ");
                }
                if (v == "-r" || v == "--run") {
                    pos_flag = true; 
                }
            }
            // 参数合法性检查, 如果 get 不到值,
            // 下面语句会使程序抛出 parameter_error 异常
            po.get("-r"); po.get("--run");
        }
        else {
            std::cerr << "[FATAL] Unsupported options" << std::endl;
            exit(-1);
        }
    }
    catch (const program_options::parameter_error& error) {
        std::cerr << "[FATAL] " << error.what() << std::endl;
        exit(-1);
    }
}

void option(int argc, char** argv, arg_t& arg) {
    assert(argv);

    program_options po("lkeep.exe [option]");    

    po.add_option("-h, --help", "Show this message.");
    po.add_option("-r, --run      <command ...>", 
            "Run a command line.\n"
            "the function finds command interpreter:\n"
            "e.g.\n"
            " * linux: '/bin/sh'\n"
            " * Windows/DOS: 'COMMAND.COM'\n"
            " * Windows NT: 'CMD.EXE'");
    po.add_option("-i, --interval <milliseconds>",
            "Milliseconds to wait between two sessions.");
    po.add_option("-m, --max      <times>",
            "Maximum number of cycles. the default value is\n"
            "infinite.");

    po.example("lkeep.exe --help");
    po.example("lkeep.exe --run echo hello world");
    po.example("lkeep.exe -i 1000 -r echo hello world");
    po.example("lkeep.exe -i 1000 -m 5 -r echo hello world");

    po.store(argc, argv);
    process_program_options(po, arg);
}

void do_loop(const arg_t& arg) {
    int loop = 0;
    while (true) {
        // TODO
        // https://github.com/BorisSchaeling/boost-process
        // http://www.highscore.de/boost/process0.5/
        // 以后要用 boost.process 替换 简陋粗糙的 ::system()
        auto ret = ::system(arg.command_line.c_str()); (void)ret;
        if (arg.interval > 0) {
            std::this_thread::sleep_for(std::chrono::milliseconds(arg.interval));
        }
        if (arg.max_cycles > 0) {
            if (++loop >= arg.max_cycles) {
                break;
            }
        }
    }
}

int main(int argc, char* argv[]) {
    arg_t arg;
    option(argc, argv, arg);

    do_loop(arg);
    return 0;
}
