#include "logrotate/logrotate.h"
#include <cstring> // for memset
#include <cstdio>  // for getchar

int main() {
    std::ofstream log;
    Logrotate lrt;
    bool ret = lrt.bind(log, "./example.log", 50/*50 byte*/);
    std::cout << "current pos = " << log.tellp() << std::endl;
    char *buf = new char[1024];
    if (ret) {
        for (int loop = 0; loop < 10; ++loop) {
            lrt.action();

            std::cout << "current pos = " << log.tellp() << std::endl;
            std::cout << "Press Enter to continue." << std::endl;
            getchar();

            std::ifstream in("./1.txt");
            if (! in)
                std::cout << "open" << std::endl;
        
            do {
                memset(buf, 0, 1024);
                in.getline(buf, 1024);
                log << buf << std::flush;
            } while(in.fail());
            
        }
    }
    else { 
        std::cout << "open file failed.\n";
    }
    delete[] buf;

    return 0;
}
