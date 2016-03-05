/*************************************************************************
	> File Name:    session_local.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/1 3:56:39
 ************************************************************************/
#include <atomic>
#include <lss/session_local.h>

using namespace lproxy::local;

/**
 * class lproxy::local::session
 */ 
session::session(boost::asio::io_service& io_service_left,
              boost::asio::io_service& io_service_right)
        : socket_left(io_service_left), socket_right(io_service_right),
//remote_ep(boost::asio::ip::address::from_string("192.168.2.109"), 6689) 
    //resolver_right(io_service_right), 
    status(status_not_connected) {
    //assert(this->auth_key.size() == 32);
    this->lss_reply.set_data_size(max_length);
}

void session::start(void) override {
    std::cout << "client: ";
    std::cout << socket_left.remote_endpoint().address() << std::endl;            
    // server
    auto& server_name = config::get_instance().get_server_name();
    auto& server_port = config::get_instance().get_server_port();
    ip::tcp::resolver resolver_right(this->socket_right.get_io_service());
    resolver_right.async_resolve({server_name, server_port}, 
            boost::bind(&session::resolve_handler, this, _1, _2));
}

tcp::socket& socket_left(void) override {
    return this->socket_left; 
}

void session::resolve_handler(const boost::system::error_code& ec,
        tcp::resolver::iterator i) {
    if (! ec) {
        boost::asio::async_connect(socket_right, i,
                boost::bind(&session::connect_handler, this, _1, _2));
    }
    else {
        // ...
        // TODO
        std::cout << "Error: " << ec.message() << "\n";
        std::cout << "主机不可达" << std::endl;
    }
}

void session::connect_handler(const boost::system::error_code& ec,
        tcp::resolver::iterator i) {
    if (! ec) {
        // 连接 server 成功
        status = status_connected; 
        
        // send hello to server
        boost::asio::async_write(socket_right, pack_hello().buffers(),
                boost::bind(&session::hello_handler, this,
                    boost::asio:placeholders::error,
                    boost::asio::placeholders::bytes_transferred)); 
    }
    else if (i != tcp::resolver::iterator()) {
        // The connection failed. Try the next endpoint in the list.
        this->socket_right.close();
        tcp::endpoint endpoint = *i;
        this->socket_right.async_connect(endpoint,
                boost::bind(&session::connect_handler, this,
                    boost::asio::placeholders::error, ++i));
    }
    else {
        // ...
        // TODO
        // 网络不可达
        std::cout << "Error: " << ec.message() << "\n";
    }
}

/*
void session::shakehands_right_write_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "shakehands_right_write_handler\n"
        "---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        socket_right.async_read_some(this->lss_reply.buffers(), 
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
*/

void session::hello_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "hello_handler \n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        status = status_hello;
        socket_right.async_read_some(this->lss_reply.buffers(), 
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

void session::exchange_handler(const boost::system::error_code& error,
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

void session::right_read_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "right_read_handler \n<--- bytes_transferred = "
        << std::dec << bytes_transferred << '\n'; 
    // </debug>
    if (! error) {
        //switch (this->lss_reply.version()) {
        //case 0x00:
        try {

            // lss包完整性检查
            lss_pack_integrity_check(bytes_transferred, this->lss_reply);

            switch (this->lss_reply.type()) {
            case reply::hello: { // 0x01
                // 状态检查
                if (status_connected != this->status) {
                    throw wrong_packet_type(); 
                }

                // 在lss_reply.data 中取出模长和公钥
                keysize_t   keysize = 0;
                //std::string public_key;
                data_t      public_key;
                //parse_hello_reply(keysize, public_key);
                unpack_reply_hello(keysize, public_key);
                // 构造 requst::exchange
                auto&& rqst_exchange = pack_exchange(keysize, public_key);
                // 发送给 server
                boost::asio::async_write(socket_right, rqst_exchange.buffers(),
                        boost::bind(&session::exchange_handler, this,
                            boost::asio:placeholders::error,
                            boost::asio::placeholders::bytes_transferred)); 




                // 生成随机数
                //std::string&& 
                this->random_str = lproxy::random_string::generate_number();
                // 组装明文 data
                /*
                std::string data_string = md5(this->auth_key) + this->random_str;
                data_t buff(data_string.begin(), data_string.end());
                */
                //std::string buff = md5(this->auth_key) + this->random_str; 
                data_t buff = md5(this->auth_key) + this->random_str;

                
                // 构造加密器
                crypto::Encryptor encryptor(
                        new crypro::Rsa(keysize, public_key));
                // 密文data
                data_t cipher; 
                encryptor.encrypt(cipher, buff.c_str(), buff.size());

                // 构造并发送 exchange key
                boost::asio::async_write(socket_right, pack_exchange(cipher).buffers(),
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
            } 
            case reply::exchange: { // 0x03
                //获取随机key this->data_key, 并验证随机数 
                if (parse_exchange(data_key)) {
                    // 通过验证
                    status = status_data;  
                    // 认证结束
                    transport(); 
                }
                else {
                    // 非法的server端
                    std::cout << "非法的server端\n";
                    delete_this();
                }
                break;
            }
            case reply::deny: // 0x04
                    std::cout << "被server端拒绝\n";
                    delete_this();
                break;
            case reply::timeout: // 0x05
                    std::cout << "server端session超时\n";
                    delete_this();
                break;
            case reply::data: { // 0x06
                    if (status_data == status) {
                        // step 1 解密
                        std::string data_right; // 解密后的 server 数据 
                        this->aes_encryptor->decrypt(data_right, 
                                lss_replay.data().str(), lss_replay.data_len());
                        // step 2 将数据送回客户端
                        boost::asio::async_write(socket_left,
                                boost::asio::buffer(data_right, bytes_transferred),
                                boost::bind(&session_local::left_write_handler, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
                    }
                    else {
                        throw wrong_packet_type();
                    }
                break;
            }
            case reply::zipdata: { // 0x17
                    if (status_data == status) {
                        // step 1 解压
                        // ...
                        //
                        // step 2 解密
                        std::string data_right; // 解密后的 server 数据 
                        this->aes_encryptor(data_right, lss_replay.data().c_str(), lss_replay.data_len());
                        // step 3 将数据送回客户端
                        boost::asio::async_write(socket_left,
                                boost::asio::buffer(data_right, bytes_transferred),
                                boost::bind(&session_local::left_write_handler, this,
                                    boost::asio::placeholders::error,
                                    boost::asio::placeholders::bytes_transferred));
                    }
                    else {
                        throw wrong_packet_type();
                    }
                break;
            }
            case reply::bad: // 0xff
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

void session::transport(void) {
    this->data_left.resize(max_length, 0);
    socket_left.async_read_some(
            boost::asio::buffer(this->data_left, max_length),
            boost::bind(&session_local::left_read_handler, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));

    socket_right.async_read_some(lss_reply.buffers(), 
            boost::bind(&session::right_read_handler, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}

void session::left_read_handler(const boost::system::error_code& error,
        size_t bytes_transferred) {
    std::cout << "left_read_handler\n";
    std::cout << "---> bytes_transferred = " 
        << std::dec << bytes_transferred << "\n";

    if (! error) {
        //this->data_left.resize(bytes_transferred, 0);
        // 对数据this->data_left[bytes_transferred] 加密
        /*
        if (! this->aes_encryptor) { 
            this->aes_encryptor = std::make_shared<crypto::Encryptor>(
                    //new crypto::Aes(auth_key));
                    new crypto::Aes(this->data_key));
        }
        */
        // 用 密文this->data_key 构造加密器
        this->aes_encryptor = std::make_shared<crypto::Encryptor>(
                new crypto::Aes(this->data_key, crypto::Aes::raw256keysetting()));
        data_t data;
        this->aes_encryptor->encrypt(data, this->data_left.c_str(), bytes_transferred);

        // 封包
        //request rqst(0x00, request::data, bytes_transferred, data);

        // if (zip_on) {
        //  auto&& rqst = pack_zipdata(data);
        // }
        // else {
            auto&& rqst = pack_data(data, bytes_transferred);
        // }

        // 发送至服务端
        boost::asio::async_write(socket_right, rqst.buffers(),
                boost::bind(&session::right_write_handler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred)); 
    }
    else {
        std::cout << "delete this " << __LINE__ << '\n';
        delete_this();
    }
}

void session::right_write_handler(const boost::system::error_code& error,
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

void session::left_write_handler(const boost::system::error_code& error,
        std::size_t bytes_trannsferred) {
    // <debug>
    std::cout << "left_write_handler\n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        socket_right.async_read_some(boost::asio::buffer(data_left, max_length),
                boost::bind(&session_local::right_read_handler, this,
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

// 组装 hello
const request& session::pack_hello(void) {
    static const request hello(0x00, request::hello, 0, data_t());
    return hello;
};
// 组装 exchange
const request session::pack_exchange(const data_t& data) {
    return request(0x00, request::exchange, data.size(), data);
}
// 组装 data
const request session::pack_data(const data_t& data, std::size_t data_len) {
    return request(0x00, request::data, data.size(), data);
}
// 组装 zipdata
const request session::pack_zipdata(const data_t& data) {
    // zip data
    // ...
    return request(0x00, request::zipdata, data.size(), data);
}
// 组装 bad
const request& session::pack_bad(void) {
    static const request bad(0x00, request::bad, 0, data_t());
    return bad;
}

// 获取pack.data中的 keysize 与 public_key
void session::parse_hello_reply(session::keysize_t& keysize, 
        std::string& public_key) {
    if (status_connected == this->status) {
        const data_t&     data     = lss_replay.data();
        const std::size_t size     = sizeof (keysize_t);
        const std::size_t data_len = lss_reply.data_len();
        assert(data_len > size);
        // TODO data.size() 不允许出现，这个检查要在外边实现
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
bool session::parse_exchange() {
    if (status_auth == this->status) {
        const std::size_t data_len = lss_reply.data_len();
        const std::string&    data = lss_reply.data(); // 当前是密文
        // TODO data.size() 不允许出现，这个检查要在外边实现
        if (data_len <= data.size()) {
            // 用this->auth_key 构造aes 加解密器
            this->aes_encryptor = std::make_shared<crypto::Encryptor>(
                    new crypto::Aes(this->auth_key));
            data_t plaintext;
            // 解密data
            this->aes_encryptor->decrypt(plaintext, data.c_str(), data.size());
            // 随机key赋值， 得到密文的 data_key
            this->data_key.assgin(plaintext.c_str(), plaintext.c_str() + 32);
            // 获取 server 端发来的随机数
            std::string random(plaintext.c_str() + 32, 
                    plaintext.c_str() + plaintext.size());

            return random == random_str;
        }
        else {
            throw incomplete_data(data_len - data.size());
        }
        return false; 
    }
    else {
        throw wrong_packet_type(); 
    }
}

void session::delete_this(void) {
    //static std::atomic_flag flag = ATOMIC_FLAG_INIT;
    if (! delete_flag.test_and_set()) {
        delete this;
        //flag = false;
    }
}


void session::unpack_reply_hello(keysize_t& keysize, data_t& public_key) {
    if (status_connected == this->status) {
        const data_t&     data     = lss_replay.data();
        const std::size_t size     = sizeof (keysize_t);
        const std::size_t data_len = lss_reply.data_len();
        assert(data_len > size);
        // TODO data.size() 不允许出现，这个检查要在外边实现
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

