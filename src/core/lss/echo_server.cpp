// http://blog.jqian.net/post/boost-asio.html#toc_0
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// http://zh.highscore.de/cpp/boost/asio.html
// http://www.cnblogs.com/zhujiechang/archive/2008/10/21/1316308.html
// http://www.cnblogs.com/kingthy/archive/2008/10/22/1317132.html
// https://zh.wikipedia.org/zh/SOCKS
// https://www.ietf.org/rfc/rfc1928.txt
// http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/socks4/


/*************************************************************************
	> File Name: echo_server.cpp
	> Author: D_L
	> Mail: deel@d-l.top
	> Created Time: 2015/11/11 12:32:00
 ************************************************************************/

//http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/echo/

// g++ -o test echo_server.cpp -I/opt/GIT/lproxy/contrib/boost/boost_1_57_0 -std=c++1y ../boost_build/link/sysprebuild.o
// http://stackoverflow.com/questions/11084296/boost-asio-async-read-some-reading-only-pieces-of-data
#ifdef __CYGWIN__ 
// 编译时必须添加宏__USE_W32_SOCKETS. 可能需要将终端编码设置成 GBK
#	include <w32api/_mingw_mac.h> // for __MSABI_LONG
#	define _WIN32_WINDOWS // or @win7: -D_WIN32_WINNT=0x0601
// g++ -o test echo_server.cpp -I/cygdrive/e/GIT/lproxy/contrib/boost/boost_1_57_0 -D__USE_W32_SOCKETS -std=c++1y  ../boost_build/link/sysprebuild.o  -lws2_32
#endif
#define BOOST_REGEX_NO_LIB
#define BOOST_DATE_TIME_SOURCE
#define BOOST_SYSTEM_NO_LIB
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/array.hpp>

#include <cstdlib>
#include <iostream>
#include <thread>

namespace lproxy {

using boost::asio::ip::tcp;

class session {
public:
    virtual void start() = 0;
    virtual tcp::socket& socket() = 0;
}; // class session

class session_local : public session {
public:
    session_local(boost::asio::io_service& io_service_left,
                  boost::asio::io_service& io_service_right)
            : socket_left(io_service_left), socket_right(io_service_right),
    remote_ep(boost::asio::ip::address::from_string("192.168.33.132"), 1081)
    {
        socket_right.connect(remote_ep);   
    }

    tcp::socket& socket() {
        return socket_left;
    }

    virtual void start() {
        std::cout << "client: ";
        std::cout << socket_left.remote_endpoint().address() << std::endl;            
        socket_left.async_read_some(boost::asio::buffer(data_left, max_length),
                boost::bind(&session_local::handle_read_left, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        socket_right.async_read_some(boost::asio::buffer(data_right, max_length),
                boost::bind(&session_local::handle_read_right, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }

private:
    void handle_read_left(const boost::system::error_code& error,
            size_t bytes_transferred) {
        std::cout << "handle_read_left " << "\n";
        std::cout << "---> bytes_transferred = " << std::dec << bytes_transferred << "\n";

        if (! error) {
            // <debug>
            /*
            for (std::size_t i = 0; i < bytes_transferred; ++i) {
                std::cout << "0x" << std::hex << (int)(data_left[i]) << " ";
            } 
            std::cout << std::endl;
            */
            //std::cout << data_left << '\n';
            std::cout << "-------------\n";

            // </debug>

            boost::asio::async_write(socket_right,
                    boost::asio::buffer(data_left, bytes_transferred),
                    boost::bind(&session_local::handle_write_right, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else {
            std::cout << "delete this " << __LINE__ << '\n';
            delete_this();
        }
    }

    void handle_write_right(const boost::system::error_code& error,
            size_t bytes_transferred) {
        std::cout << "handle_write_right " << "\n";
        std::cout << "---> bytes_transferred = " << std::dec << bytes_transferred << "\n";
        if (! error) {
            socket_left.async_read_some(boost::asio::buffer(data_left, max_length),
                    boost::bind(&session_local::handle_read_left, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else {
            std::cout << "delete this " << __LINE__ << '\n';
            delete_this();
        }
    }

    void handle_read_right(const boost::system::error_code& error,
            size_t bytes_transferred) {
        std::cout << "handle_read_right " << "\n";
        std::cout << "<--- bytes_transferred = " << std::dec << bytes_transferred << "\n";
        if (! error) {
            // <debug>
            /*
            for (std::size_t i = 0; i < bytes_transferred; ++i) {
                std::cout << "0x" << std::hex << (int)(data_right[i]) << " ";
            } 
            std::cout << std::endl;
            */
            //std::cout << data_right << '\n';
            std::cout << "-------------\n";
            // </debug>

            boost::asio::async_write(socket_left,
                    boost::asio::buffer(data_right, bytes_transferred),
                    boost::bind(&session_local::handle_write_left, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else {
            std::cout << "delete this " << __LINE__ << '\n';
            delete_this();
        }
    }

    void handle_write_left(const boost::system::error_code& error,
            size_t bytes_transferred) {
        std::cout << "handle_write_left " << "\n";
        std::cout << "---> bytes_transferred = " << std::dec << bytes_transferred << "\n";
        if (! error) {
            socket_right.async_read_some(boost::asio::buffer(data_right, max_length),
                    boost::bind(&session_local::handle_read_right, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else {
            std::cout << "delete this " << __LINE__ << '\n';
            delete_this();
        }
    }

    void delete_this(void) {
        static bool flag = true;
        if (flag) {
            delete this;
            flag = false;
        }
    }

private:
    tcp::socket   socket_left;  // client 
    tcp::socket   socket_right; // remote
    tcp::endpoint remote_ep; // ??????
    enum          { max_length = 2048 };
    char          data_left[max_length];
    char          data_right[max_length];
}; // class session_local


namespace socks5 {
static const unsigned char version = 0x05;
/*
class request {
private:
    char version_;
    char nMethods_;
    //char 

};*/
class request {
public:
    enum command_type {
        connect = 0x01,
        bind    = 0x02
    };

}; // class request

class reply {
public:

}; // class replay

} // namespace socks5 

class session_server : public session {
public:

}; // session_server

class ss_server {
public:
    ss_server(boost::asio::io_service& io_service, short port)
            : io_service_(io_service),
            acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
        start_accept();
        std::thread tright(handle_thread_right, std::ref(io_service_right()));
        this->thread_right = std::move(tright);
    }

private:
    void start_accept() {
        session* new_session = 
            new session_local(io_service_left(), io_service_right());
        acceptor_.async_accept(new_session->socket(),
                boost::bind(&ss_server::handle_accept, this, new_session,
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
        std::cout << "thread id = " << this_id << " start..\n";
        // 为了不使"ios没有任务, ios.run就立刻返回"
        boost::asio::io_service::work work(io_service); 
        io_service.run(); 
        std::cout << "thread id = " << this_id << " exit!\n";
    }
private:
    boost::asio::io_service& io_service_;
    tcp::acceptor            acceptor_;
    std::thread              thread_right;
}; // class ss_server

} // namespace lproxy

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: async_tcp_echo_server <port>\n";
            return 1;
        }

        boost::asio::io_service io_service;

        using namespace std; // For atoi.
        lproxy::ss_server s(io_service, atoi(argv[1]));

        io_service.run();
    }
    catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << "\n";
    }

    return 0;
}
