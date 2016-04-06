/*************************************************************************
	> File Name:    main.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/29 9:22:11
 ************************************************************************/

#include <iostream>
#include <assert.h>
#include <cstdlib> // for exit()
#include "../program_options.h"

void process_program_options(const program_options& po
        /*, get_option*/) {
    if (po.empty()) {
        std::string message = po.show_help("arg empty");
        std::cout << message;
        exit(0);
    }
    if (po.count("-h") || po.count("--help")) {
        std::string message = po.show_help("this is title");
        std::cout << message;
        exit(0);
    }
    if (po.count("-v") || po.count("--version")) {
        std::cout << "ver: 0.1" << std::endl;
        exit(0);
    }
    /*
    if (po.count("-s") || po.count("--sizeof-this")) {
        std::cout << "sizeof (program_options) = " << sizeof (program_options)
            << std::endl;
        std::cout << "sizeof (int) = " << sizeof (int) << std::endl;
        std::cout << "sizeof (void*) = " << sizeof (void*) << std::endl;
        exit(0);
    }
    */
    if (po.count("-c") || po.count("--command")) {
        for (auto& v : po.get_argv()) {
            std::cout << v << " ";
        }
        std::cout << std::endl;
        exit(0);
    }
    try {
        if (po.count("-p")) {
            std::string value = po.get("-p");
            std::cout << value << std::endl;
            return;
        }
        if (po.count("--print")) {
            std::string value = po.get("--print");
            std::cout << value << std::endl;
            return;
        }
    }
    catch (const program_options::parameter_error& error) {
        std::cerr << error.what() << std::endl;
        exit(-1);
    }

    std::cerr << "Unsupported options" << std::endl;
    exit(1);
}

void option(int argc, char** argv /*, get_option*/) {
    assert(argv);
    program_options po("demo [option]");
    po.add_option("-h, --help", "Show this message.");
    po.add_option("-v, --version", "Show current version.");
    po.add_option("-p, --print   <something>", "Print a message.\ne.g. demo -p hello");
    po.add_option("-c, --command", "Print current command.");
    //po.add_option("-s, --sizeof-this", "printf(\"%d\", sizeof (program_options));");

    po.example("demo --help");
    po.example("demo -v");
    po.example("demo -p helloworld");
    po.example("demo -c");

    po.store(argc, argv);

    process_program_options(po /*, get_option*/);
}

int main(int argc, char* argv[]) {
    option(argc, argv /*, get_option*/);

    // do something
    return 0;
}

/*

$ make clean; make

$ ./bin/demo
arg empty

Usage: demo [option]

Options:
  -h, --help                 Show this message.
  -v, --version              Show current version.
  -p, --print   <something>  Print a message.
                             e.g. demo -p hello
  -c, --command              Print current command.

Examples:
  demo --help
  demo -v
  demo -p helloworld
  demo -c


$ ./bin/demo -h
this is title

Usage: demo [option]

Options:
  -h, --help                 Show this message.
  -v, --version              Show current version.
  -p, --print   <something>  Print a message.
                             e.g. demo -p hello
  -c, --command              Print current command.

Examples:
  demo --help
  demo -v
  demo -p helloworld
  demo -c


$ ./bin/demo --version
ver: 0.1

$ ./bin/demo -p helloworld
helloworld

$ ./bin/demo -p
Exception: program_options::parameter_error: -p

$ ./bin/demo -xxxxx
Unsupported options

$ ./bin/demo -c
./bin/demo -c

*/
