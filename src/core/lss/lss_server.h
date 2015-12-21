#ifndef _LSS_SERVER_H_1 
#define _LSS_SERVER_H_1
/*************************************************************************
	> File Name:    lss_server.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/30 5:17:21
 ************************************************************************/
#include <type_traits>

namespace lproxy {

template <typename SESSION>
class lss_server {
static_assert(std::is_base_of<session, SESSION>::value, 
        "SESSION should be derived from 'session'.");
public:
    lss_server(boost::asio::io_service& io_service, short port)
            : io_service_(io_service),
            acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
        std::thread tright(handle_thread_right, std::ref(io_service_right()));
        this->thread_right = std::move(tright);
    }

private:
    void start_accept() {
        session* new_session = 
            new SESSION(io_service_left(), io_service_right());
        acceptor_.async_accept(new_session->socket(),
                boost::bind(&lss_server::handle_accept, this, new_session,
                    boost::asio::placeholders::error));
    }

    void handle_accept(session* new_session,
            const boost::system::error_code& error) {
        if (! error) {
            new_session->start();
        }
        else {
            delete new_session;
        }

        start_accept();
    }

    boost::asio::io_service& io_service_left(void) {
        return this->io_service_;
    }
    boost::asio::io_service& io_service_right(void) {
        static boost::asio::io_service io_service;
        return io_service;
    }
    static void handle_thread_right(boost::asio::io_service& io_service) {
        std::thread::id this_id = std::this_thread::get_id();
        // <debug>
        std::cout << "thread id = " << this_id << " start..\n";
        // </debug>
        // 为了不使"ios没有任务, ios.run就立刻返回"
        boost::asio::io_service::work work(io_service); 
        io_service.run(); 
        // <debug>
        std::cout << "thread id = " << this_id << " exit!\n";
        // </debug>
    }
private:
    boost::asio::io_service& io_service_;
    tcp::acceptor            acceptor_;
    std::thread              thread_right;
}; // class lss_server


namespace local { 
    typedef lss_server<local::session>  lss_server;
} // namespace lproxy::local

namespace server {
    typedef lss_server<server::session> lss_server;
} // namespace lproxy::server

} // namespace lproxy

#endif // _LSS_SERVER_H_1



