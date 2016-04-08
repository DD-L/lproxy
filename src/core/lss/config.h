#ifndef CONFIG_H_1
#define CONFIG_H_1
/*************************************************************************
	> File Name:    config.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/1 7:47:37
 ************************************************************************/
#include <lss/typedefine.h>
#include <lss/log.h>
#include <thread>
#include <chrono>

namespace lproxy {

class config {
    virtual void configure(const sdata_t& config_file) = 0;
public:
    virtual ~config(void) {}
    static void signal_handler(boost::asio::io_service* ios,
            const boost::system::error_code& err, 
            int signal) {
        if (err)   return;
        if (! ios) return;
        switch (signal) {
            case SIGINT:
            case SIGTERM:
                _print_s("Exit..");
                ios->stop();
                while (! ios->stopped()) {
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                }
                exit(0);
        default:
            break;
        } 
    } 
}; // class config
} // namespace lproxy

#endif // CONFIG_H_1

