#ifndef _LSS_SERVER_H_1 
#define _LSS_SERVER_H_1
/*************************************************************************
	> File Name:    lss_server.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 5:17:21
 ************************************************************************/
#include <type_traits>
#include <thread>
#include <lss/config.h>
#include <lss/session.h>
#include <lss/log.h>


namespace lproxy {

// lproxy::lss_server
template <typename SESSION>
class lss_server {
static_assert(std::is_base_of<session, SESSION>::value, 
        "SESSION should be derived from 'session'.");
public:
    lss_server(boost::asio::io_service& io_service, 
            const sdata_t& bind_addr, const uint16_t bind_port)
            : io_service_(io_service), 
            acceptor_(io_service, {
                    ip::address::from_string(bind_addr), bind_port}) {
        loginfo("bind addr: " << bind_addr << " bind port: " << bind_port);
        start_accept();
        std::thread thread_right(handle_thread_right, 
                std::ref(io_service_right()));
        this->thread_right = std::move(thread_right);

        // register signal
        boost::asio::signal_set sig_left(io_service_left(), SIGINT, SIGTERM);
        boost::asio::signal_set sig_right(io_service_right(), SIGINT, SIGTERM);
        sig_left.async_wait(boost::bind(&config::signal_handler, 
                    &io_service_left(), _1, _2));
        sig_right.async_wait(boost::bind(&config::signal_handler, 
                    &io_service_right(), _1, _2));
    }
    virtual ~lss_server() {
// Program received signal SIGABRT, Aborted.
//0x00007ffff6dfbcc9 in raise () from /lib/x86_64-linux-gnu/libc.so.6
//(gdb) bt
//#0  0x00007ffff6dfbcc9 in raise () from /lib/x86_64-linux-gnu/libc.so.6
//#1  0x00007ffff6dff0d8 in abort () from /lib/x86_64-linux-gnu/libc.so.6
//#2  0x00007ffff77106dd in __gnu_cxx::__verbose_terminate_handler() () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
//#3  0x00007ffff770e746 in ?? () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
//#4  0x00007ffff770e791 in std::terminate() () from /usr/lib/x86_64-linux-gnu/libstdc++.so.6
//#5  0x000000000041b4eb in std::thread::~thread (this=0x7fffffffe3a8, __in_chrg=<optimized out>) at /usr/include/c++/4.9/thread:146
//#6  0x000000000041ba60 in lproxy::lss_server<lproxy::local::session>::~lss_server (this=0x7fffffffe380, __in_chrg=<optimized out>)
//    at /opt/lproxy/src/core/lss/../../..//src/core/lss/lss_server.h:20
//#7  0x0000000000412cf1 in main (argc=1, argv=0x7fffffffe608) at local.cpp:29
 
        // TODO
        // 尝试修复上述bug:
        // 析构前添加 thread::detach(), 使 joinable == false, 
        // 避免析构 this->thread_right 时, std::terminate() 被调用
        this->thread_right.detach();
    }

    void stop(void) {
        io_service_right().stop();
        loginfo("Stopping io_service_right...");
        while (! io_service_right().stopped()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        io_service_left().stop();
        loginfo("Stopping io_service_left...");
        while (! io_service_left().stopped()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        stopped_ = true;
        loginfo("lss_server stopped.");
    }

    bool stopped(void) const {
        return stopped_;
    }
private:
    void start_accept() {
        /*
        session* new_session = 
            new SESSION(io_service_left(), io_service_right());
        */
        /*
        session::pointer new_session = 
            new SESSION(io_service_left(), io_service_right());
        */
        session::pointer new_session = 
            std::make_shared<SESSION>(io_service_left(), io_service_right());
        acceptor_.async_accept(new_session->get_socket_left(),
                boost::bind(&lss_server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
    }

    void handle_accept(session::pointer new_session,
            const boost::system::error_code& error) {
        if (! error) {
            new_session->start();
        }
        else {
            /*
            delete new_session;
            new_session = nullptr;
            */
            new_session->close();
        }

        start_accept();
    }

    // thread function
    static void handle_thread_right(boost::asio::io_service& io_service) {
        lsslogdebug("thread io_service_right start..");

        // 为了不使"ios没有任务, ios.run就立刻返回"
        boost::asio::io_service::work work(io_service); 
        for (;;) {
            try {
                io_service.run();
                break;
            }
            catch (boost::system::system_error const& e) {
                logerror(e.what());
            }
            catch (const std::exception& e) {
                logerror(e.what());
            }
            catch (...) {
                logerror("An error has occurred. io_service_right.run()");
            }
        }

        lsslogdebug("thread io_service_right exit!");
    }

    boost::asio::io_service& io_service_left(void) {
        return this->io_service_;
    }
    boost::asio::io_service& io_service_right(void) {
        static boost::asio::io_service io_service;
        return io_service;
    }

private:
    bool                     stopped_ = false;
    boost::asio::io_service& io_service_;
    tcp::acceptor            acceptor_;
    std::thread              thread_right;
}; // class lproxy::lss_server



} // namespace lproxy

#include <lss/session_local.h>  // for  lproxy::local::session
#include <lss/session_server.h> // for lproxy::server::session
namespace lproxy {
namespace local { 
    typedef lproxy::lss_server<local::session>  lss_server;
} // namespace lproxy::local

namespace server {
    typedef lproxy::lss_server<server::session> lss_server;
} // namespace lproxy::server
} // namespace lproxy

#endif // _LSS_SERVER_H_1

