#ifndef _SESSION_LOCAL_H_1
#define _SESSION_LOCAL_H_1
/*************************************************************************
	> File Name:    session_local.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/1 3:56:39
 ************************************************************************/
#include <atomic>
#include "lss/session.h"

namespace lproxy {


namespace local {

// 自由函数
class __random {
public:
    random(void) {
        srand((unsigned)time(NULL));
    }
    void clear(void) {}
};
static std::string gen_random_string(void) {
    static __random r; r.clear();
    byte random_array[10];
    for (int i = 0; i < 10; ++i) {
        random_array[i] = rand() % 10 + '0';
    }
    return std::string(random_array, random_array + 10);
} 

class session : public lproxy::session {
public:
    session_local(boost::asio::io_service& io_service_left,
                  boost::asio::io_service& io_service_right)
        :socket_left(io_service_left), socket_right(io_service_right),
    //remote_ep(boost::asio::ip::address::from_string("192.168.2.109"), 6689) 
        resolver_right(io_service_right), status(status_not_connected) {
            //assert(this->auth_key.size() == 32);
        }


    virtual void start(void) override {
        std::cout << "client: ";
        std::cout << socket_left.remote_endpoint().address() << std::endl;            
        
        const char* host = "192.168.2.109"; // ?
        const char* port = "6689";
        socket_right.async_resolve({host, port}, 
                boost::bind(&session::resolve_handler, this, _1, _2));
    }
private:
    void resolve_handler(const boost::system::error_code& ec,
            tcp::resolver::iterator i) {
        if (! ec) {
            boost::asio::async_connect(socket_right, i,
                    boost::bind(&session::connect_handler, this, _1, _2));
        }
        else {
            // ...
        }
    }

    void connect_handler(const boost::system::error_code& ec,
            tcp::resolver::iterator i) {
        if (! ec) {
            status = status_connected; 
            
            // send hello
            boost::asio::async_write(socket_right, hello().buffers(),
                    boost::bind(&session::hello_handler, this,
                        boost::asio:placeholders::error,
                        boost::asio::placeholders::bytes_transferred)); 
        }
        else {
            // ...
        }
    }

    // 备忘
    // hello_handler 与 exchange_handler 
    // 都可以合并成 shakehands_right_write_handler
    // 最新： 不建议合并
    void shakehands_right_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred) {
        // <debug>
        std::cout << "shakehands_right_write_handler\n"
            "---> bytes_transferred = "
            << std::dec << bytes_transferred << '\n';
        // </debug>
        if (! error) {
            socket_right.async_read_some(lss_reply.buffers(), 
                    boost::bind(&session::right_read_handler, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else {
            // <debug>
            std::cout << "delete this " << __LINE__ << '\n';
            // </debug>
            delete_this();
        }

    }

    void hello_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred) {
        // <debug>
        std::cout << "hello_handler \n---> bytes_transferred = "
            << std::dec << bytes_transferred << '\n';
        // </debug>
        if (! error) {
            status = status_hello;
            socket_right.async_read_some(lss_reply.buffers(), 
                    boost::bind(&session::right_read_handler, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else {
            // <debug>
            std::cout << "delete this " << __LINE__ << '\n';
            // </debug>
            delete_this();
        }

    }
    void exchange_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred) {
        // <debug>
        std::cout << "exchange_handler \n---> bytes_transferred = "
            << std::dec << bytes_transferred << '\n';
        // </debug>
        if (! error) {
            status = status_auth;
            socket_right.async_read_some(lss_reply.buffers(), 
                    boost::bind(&session::right_read_handler, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else {
            // <debug>
            std::cout << "delete this " << __LINE__ << '\n';
            // </debug>
            delete_this();
        }

    }

    void right_read_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred) {
        // <debug>
        std::cout << "right_read_handler \n<--- bytes_transferred = "
            << std::dec << bytes_transferred << '\n'; 
        // </debug>
        if (! error) {
            //switch (lss_reply.version()) {
            //case 0x00:
            try {
                switch (lss_reply.type()) {
                case reply::hello:

                    // 在lss_reply.data 中取出模长和公钥
                    keysize_t   keysize = 0;
                    std::string public_key;
                    parse_hello_reply(keysize, public_key);

                    // 生成随机数
                    std::string random_string = gen_random_string();
                    // 组装明文 data
                    std::string data_string = md5(this->auth_key) + random;
                    data_t buff(data_string.begin(), data_string.end());

                    
                    // 构造加密器
                    crypto::Encryptor encryptor(
                            new crypro::Rsa(keysize, public_key));
                    // 密文data
                    data_t cipher; 
                    encryptor.encrypt(cipher, buff, buff.size());

                    // 构造并发送 exchange key
                    boost::asio::async_write(socket_right, exchange(cipher).buffers(),
                            boost::bind(&session::exchange_handler, this,
                                boost::asio:placeholders::error,
                                boost::asio::placeholders::bytes_transferred)); 
                    /**
                    http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/socks4/socks4.hpp
                    http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/socks4/sync_client.cpp
                    http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
                    http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_client.cpp
                    http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/example/cpp03/echo/async_tcp_echo_server.cpp
                     */
                    
                    break;
                case reply::exchange:
                    //获取随机key, 并验证随机数 
                    std::string data_key;
                    if (parse_exchange(data_key)) {
                        // 通过验证
                        status = status_data;  
                        // 认证结束
                        trans(); 
                    }
                    else {
                        // 非法的server端
                        std::cout << "非法的server端\n";
                        delete_this();
                    }
                    break;
                case reply::deny:
                        std::cout << "被server端拒绝\n";
                        delete_this();
                    break;
                case reply::timeout:
                        std::cout << "server端session超时\n";
                        delete_this();
                    break;
                case reply::data:
                    ?????????????? trans??
                        if (status_data == status) {
                            // step 1 解密
                            //
                            // step 2 将数据送回客户端
                            boost::asio::async_write(socket_left,
                                    boost::asio::buffer(data_right, bytes_transferred),
                                    boost::bind(&session_local::?????????????????????????????????????????????????????handle_write_left, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
                        }
                        else {
                            throw wrong_packet_type();
                        }
                    break;
                case reply::zipdata:
                    ?????????????????? trans??
                        if (status_data == status) {
                            trans(); 
                            // step 1 解压
                            // step 2 解密
                            // step 3 将数据送回客户端
                        }
                        else {
                            throw wrong_packet_type();
                        }
                    break;
                case reply::bad:
                default:
                        std::cout << "数据包bad\n";
                        delete_this();
                    break;
                }
            }
            catch (wrong_packet_type&) {
                // default:
            }
            catch (incomplete_data& ec) {
                // 不完整数据
                // 少了 ec.less() 字节
            }
            //break;}
        }
        else {
            // <debug>
            std::cout << "delete this " << __LINE__ << '\n';
            // </debug>
            delete_this();
        }
    }

    void trans(void) {
        socket_left.async_read_some(boost::asio::buffer(data_left, max_length),
                boost::bind(&session_local::left_read_handler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));

        socket_right.async_read_some(lss_reply.buffers(), 
                boost::bind(&session::right_read_handler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    
    }

    void left_read_handler(const boost::system::error_code& error,
            size_t bytes_transferred) {
        std::cout << "left_read_handler\n";
        std::cout << "---> bytes_transferred = " 
            << std::dec << bytes_transferred << "\n";

        if (! error) {
            // 对数据data_left[bytes_transferred] 加密
            if (! aes_encryptor) {
                aes_encryptor = std::make_shared<crypto::Encryptor>(
                        new crypto::Aes(auth_key));
            }
            data_t data;
            aes_encryptor->encrypt(data, data_left, bytes_transferred);

            // 封包
            request rqst(0x00, request::data, bytes_transferred, data);

            // 发送至服务端
            boost::asio::async_write(socket_right, rqst.buffers(),
                    boost::bind(&session::right_write_handler, this,
                        boost::asio:placeholders::error,
                        boost::asio::placeholders::bytes_transferred)); 
        }
        else {
            std::cout << "delete this " << __LINE__ << '\n';
            delete_this();
        }
    }

    // ???????????????????????
    void right_write_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred) {
        // <debug>
        std::cout << "right_write_handler\n---> bytes_transferred = "
            << std::dec << bytes_transferred << '\n';
        // </debug>
        if (! error) {
            socket_left.async_read_some(boost::asio::buffer(data_left, max_length),
                    boost::bind(&session_local::left_read_handler, this,
                        boost::asio::placeholders::error,
                        boost::asio::placeholders::bytes_transferred));
        }
        else {
            // <debug>
            std::cout << "delete this " << __LINE__ << '\n';
            // </debug>
            delete_this();
        }

    }


private:
    class wrong_packet_type : public std::exception {
    public:
        wrong_packet_type(void) noexcept : std::exception() {}
        virtual ~wrong_packet_type(void) noexcept {}
        virtual const char* what(void) const noexcept {
            return "wrong_packet_type";
        } 
    }
    class incomplete_data : public std::exception {
    public:
        incomplete_data(data_len_t less) noexcept : less_(less) {}
        virtual ~incomplete_data(void) noexcept {}
        virtual const char* what(void) const noexcept {
            return "incomplete_data";
        }
        const data_len_t less(void) {
            return less_;
        }
        const data_len_t less(void) const {
            return const_cast<decltype(this)>(this)->less();
        }
    private:
        uint16_t    less_;
    }
private:
    const request& hello(void) {
        static const request hello(0x00, request::hello, 0, data_t());
        return hello;
    };
    const request exchange(const data_t& data) {
        return request(0x00, request::exchange, data.size(), data);
    }

private:
    typedef uint16_t    keysize_t;
    // 获取pack.data中的 keysize 与 public_key
    void parse_hello_reply(keysize_t& keysize, std::string& public_key) {
        if (status_connected == this->status) {
            const data_t&     data     = lss_replay.data();
            const std::size_t size     = sizeof(keysize_t);
            const std::size_t data_len = lss_reply.data_len();
            assert(data_len > size);
            if (data_len <= data.size()) {
                keysize = data[0];
                keysize = ((keysize << 8) & 0xff00) | data[1];
                public_key.assign(&data[size], &data[data_len - size]);
            }
            else {
                throw incomplete_data(data_len - data.size());
            }
        }
        else {
            throw wrong_packet_type(); 
        }
    }
    // 获取随机key, 并验证服务端
    bool parse_exchange(std::string& data_key) {
        if (status_auth == this->status) {
            bool ret = false;
            const std::size_t data_len = lss_reply.data_len();
            const data_t& data         = lss_reply.data(); // 当前是密文
            if (data_len <= data.size()) {
                // 构造aes 加解密器
                aes_encryptor = std::make_shared<crypto::Encryptor>(
                        new crypto::Aes(auth_key));
                data_t plaintext;
                // 解密data
                aes_encryptor->decrypt(plaintext, data, data.size());
                // 随机key赋值
                data_key.assgin(&plaintext[0], &plaintext[0] + 32);

                std::string random(&plaintext[0] + 32, 
                        &plaintext[0] + plaintext.size());

                return random == random_string;

            }
            else {
                throw incomplete_data(data_len - data.size());
            }
            return ret; 
        }
        else {
            throw wrong_packet_type(); 
        }
    }

private:
    virtual tcp::socket& socket(void) override {
        return socket_left; 
    }
    void delete_this(void) {
        //static std::atomic_flag flag = ATOMIC_FLAG_INIT;
        if (! delete_flag.test_and_set()) {
            delete this;
            flag = false;
        }
    }
private:
    enum {
        status_not_connected = 0,
        status_connected     = 1,
        status_hello         = 2,
        status_auth          = 3,
        status_data          = 4
    } status; 

    reply              lss_reply;
    const std::string& auth_key = "xxxxxx"; // 明文
    std::string        random_string;
    std::shared_ptr<crypto::Encryptor> aes_encryptor;
    std::string        data_key;
private:
    tcp::socket      socket_left;  // client 
    tcp::socket      socket_right; // remote
    //tcp::endpoint  remote_ep; // ??????
    tcp::resolver    resolver_right;
    std::atomic_flag delete_flag = ATOMIC_FLAG_INIT;
}; // class lproxy::local::session

} // namespace lproxy::local
} // namespace lproxy

#endif // _SESSION_LOCAL_H_1
