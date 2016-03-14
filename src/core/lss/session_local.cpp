/*************************************************************************
	> File Name:    session_local.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/1 3:56:39
 ************************************************************************/
#include <atomic>
#include <lss/session_local.h>
#include <lss/config.h>
#include <crypto/md5_crypto.h>
#include <crypto/rsa_crypto.h>
#include <except/except.h>

using namespace lproxy::local;
using lproxy::tcp;

/**
 * class lproxy::local::session
 */ 
session::session(boost::asio::io_service& io_service_left,
              boost::asio::io_service& io_service_right)
        : socket_left(io_service_left), socket_right(io_service_right),
        resolver_right(io_service_right)
{
    status = status_not_connected;    
    // 是否 发心跳?
    // TODO
    //ip::tcp::socket::keep_alive keepalive(true);
    //socket_right.set_option(keepalive);
}

void session::start(void) {
    std::cout << "client: ";
    std::cout << socket_left.remote_endpoint().address() << std::endl;            
    // server
    const auto& server_name = config::get_instance().get_server_name();
    const auto& server_port = config::get_instance().get_server_port();

    std::cout << "async_resolve: " << server_name << ":" << server_port << std::endl;
    this->resolver_right.async_resolve({server_name, server_port}, 
            boost::bind(&session::resolve_handler, shared_from_this(), _1, _2));
}

void session::close(void) {
    //static std::atomic_flag flag = ATOMIC_FLAG_INIT;
    if (! close_flag.test_and_set()) {
        // TODO
        // step 1
        // http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/reference/basic_stream_socket/cancel/overload1.html
        // 
        // cancel session 上所有的异步
        socket_left.close();
        socket_right.close();
        
        // step 2
        //delete this;
        //flag = false;
    }
}

tcp::socket& session::get_socket_left(void) {
    return this->socket_left; 
}

void session::resolve_handler(const boost::system::error_code& ec,
        tcp::resolver::iterator i) {
    if (! ec) {
        boost::asio::async_connect(socket_right, i,
                boost::bind(&session::connect_handler, 
                    shared_from_this(), _1, _2));
    }
    else {
        // ...
        // TODO
        std::cout << "Error: " << ec.message() << "\n";
        std::cout << "主机不可达" << std::endl;
        std::cout << "close this " << __LINE__ << '\n';
        //delete_this();
        this->close();
    }
}

void session::connect_handler(const boost::system::error_code& ec,
        tcp::resolver::iterator i) {
    if (! ec) {
        // 连接 server 成功
        status = status_connected; 
        
        // send hello to server
        boost::asio::async_write(socket_right, pack_hello().buffers(),
                boost::bind(&session::hello_handler, 
                    shared_from_this(), _1, _2));
                    //boost::asio::placeholders::error,
                    //boost::asio::placeholders::bytes_transferred)); 
        
        // debug
        std::cout << "send hello to server:" << std::endl;
        _debug_print_data(get_vdata_from_lss_pack(pack_hello()), int(),
                ' ', std::hex);
        // debug
    }
    else if (i != tcp::resolver::iterator()) {
        // The connection failed. Try the next endpoint in the list.
        this->socket_right.close();
        tcp::endpoint endpoint = *i;
        this->socket_right.async_connect(endpoint,
                boost::bind(&session::connect_handler, shared_from_this(),
                    boost::asio::placeholders::error, ++i));
    }
    else {
        // ...
        // TODO
        // 网络不可达
        std::cout << "Error: " << ec.message() << "\n";
        std::cout << "网络不可达" << std::endl;
        std::cout << "close this " << __LINE__ << '\n';
        this->close();
    }
}


void session::hello_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "hello_handler \n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        // session 中第一次用 this->lss_reply 读数据, 需要开辟空间
        //this->lss_reply.set_data_size(max_length);
        // or
        //this->lss_reply.assign_data(max_length, 0);

        auto&& lss_reply = make_shared_reply();

        this->socket_right.async_read_some(lss_reply->buffers(), 
                boost::bind(&session::right_read_handler, shared_from_this(),
                    _1, _2, lss_reply));
                    //boost::asio::placeholders::error,
                    //boost::asio::placeholders::bytes_transferred));
        this->status = status_hello;
    }
    else {
        // <debug>
        std::cout << "close this " << __LINE__ << '\n';
        // </debug>
        //delete_this();
        this->close();
    }
}

void session::exchange_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_request_type lss_request) {
    (void)lss_request;
    // <debug>
    std::cout << "exchange_handler \n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        // 每次异步读数据前，清空 data
        //this->lss_reply.assign_data(max_length, 0);
        auto&& lss_reply = make_shared_reply();
        this->socket_right.async_read_some(lss_reply->buffers(), 
                boost::bind(&session::right_read_handler, shared_from_this(), 
                    _1, _2, lss_reply));
                    //boost::asio::placeholders::error,
                    //boost::asio::placeholders::bytes_transferred));
        status = status_auth;
    }
    else {
        // <debug>
        std::cout << "close this " << __LINE__ << '\n';
        // </debug>
        //delete_this();
        this->close();
    }
}

void session::right_read_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_reply_type lss_reply) { 
    // <debug>
    std::cout << "right_read_handler \n<--- bytes_transferred = "
        << std::dec << bytes_transferred << '\n'; 
    std::cout << "lss_reply->version() = "
        << (int)lss_reply->version() << '\n';
    std::cout << "lss_reply->type() = "
        << (int)lss_reply->type() << '\n';
    std::cout << "lss_reply->data_len() = "
        << lss_reply->data_len() << '\n';
    _debug_print_data(get_vdata_from_lss_pack(*lss_reply),
            int(), ' ', std::hex);
    // </debug>
    if (! error) {
        //switch (lss_reply->version()) {
        //case 0x00:
        try {
            bool is_zip_data = false;

            // lss包完整性检查
            lss_pack_integrity_check(bytes_transferred, *lss_reply);

            switch (lss_reply->type()) {
            case reply::hello: { // 0x01
                // 状态检查
                /*
                if (status_hello != this->status) {
                    throw wrong_packet_type(); 
                }
                */
                // 在lss_reply->data 中取出模长和公钥
                keysize_t   keysize = 0;
                data_t      public_key;
                unpack_reply_hello(keysize, public_key, *lss_reply);
                // 构造 requst::exchange
                //auto&& exchange = pack_exchange(keysize, public_key);

                // test
                auto&& exchange_request = make_shared_request(
                        pack_exchange(keysize, public_key));
                boost::asio::async_write(this->socket_right, 
                        exchange_request->buffers(),
                        boost::bind(&session::exchange_handler, 
                            shared_from_this(), _1, _2, exchange_request));

                // debug
                std::cout << "send exchange to server: " << std::endl;
                _debug_print_data(get_vdata_from_lss_pack(*exchange_request), 
                        int(), ' ', std::hex);
                break;
            } 
            case reply::exchange: { // 0x03
                /*
                if (status_auth != this->status) {
                    throw wrong_packet_type(); 
                }
                */
                sdata_t reply_random_str;
                // 解包，获取 随机数 和 随机key
                unpack_reply_exchange(reply_random_str, *lss_reply);
                // 验证 随机 数
                if (reply_random_str != this->random_str) {
                    std::cout << "非法的server端\n";
                    //delete_this();
                    this->close();
                    break; 
                }

                std::cout << "验证通过" << std::endl;

                // 验证通过
                status = status_data;
                transport(); 
                break;
            }
            case reply::deny: // 0x04
                    std::cout << "被server端拒绝\n";
                    //delete_this();
                    this->close();
                break;
            case reply::timeout: // 0x05
                    std::cout << "server端session超时\n";
                    //delete_this();
                    this->close();
                break;
            case reply::zipdata:// 0x17
                is_zip_data = true;
            case reply::data: { // 0x06
                    if (status_data == status) {
                        // 解包
                        data_t data_right;
                        const int rest_lss_data_len = 
                            unpack_reply_data(data_right, bytes_transferred, 
                                    *lss_reply, is_zip_data);

                        /*
                        std::cout << "after unpack_reply_data, data_right = \n";
                        _debug_print_data(data_right, int(), ' ', std::hex);
                        */

                        if (rest_lss_data_len < 0) {
                            throw incomplete_data(0 - rest_lss_data_len); 
                        }

                        std::shared_ptr<data_t> data 
                            = std::make_shared<data_t>(data_right);

                        // 裁剪 lss_reply 数据，（当前数据已缓存到*data）
                        // 减掉当前lss数据。 (分包)
                        bool is_continue = 
                            cut_lss(bytes_transferred - rest_lss_data_len,
                                bytes_transferred, *lss_reply);
                        // bytes_transferred - rest_lss_data_len 的意义是 
                        // 新包在 旧包中 开始的位置（0开头）

                        /*
                        std::cout << "after cut_lss, *data = " << std::endl;
                        _debug_print_data(*data, int(), ' ', std::hex);
                        */

                        // 将 data_right 发至 client
                        if (rest_lss_data_len > 0 && is_continue) {
                            // lss_reply 里还有未处理的数据
                            std::cout << "unprocessed data still in lss_reply.."
                                << std::endl;

                            boost::asio::async_write(this->socket_left,
                                boost::asio::buffer(*data),
                                boost::bind(&session::right_read_handler, 
                                    shared_from_this(),
                                    boost::asio::placeholders::error,
                                    std::size_t(rest_lss_data_len), lss_reply));
                        }
                        else {
                            // 最后一条不可再分割的数据再绑定 left_write_handler
                            boost::asio::async_write(this->socket_left,
                                 boost::asio::buffer(*data),
                                 boost::bind(&session::left_write_handler,
                                   shared_from_this(), _1, _2)); 
                        }

                        std::cout << "send plain data to client : ";
                        _debug_print_data(*data, int(), ' ', std::hex);
                        _debug_print_data(*data, char(), 0);
                        std::cout << std::endl;
                    }
                    else {
                        std:: cout << "reply::data: status Wrong" << std::endl;
                        throw wrong_packet_type();
                    }
                break;
            }
            case reply::bad: // 0xff
            default:
                    std::cout << "数据包bad\n";
                    //delete_this();
                    this->close();
                break;
            }
        }
        catch (wrong_packet_type&) {
            // default:
            // TODO
            std::cout << "wrong_packet_type" << std::endl;
            // 临时方案
            //delete_this();
            this->close();
        }
        catch (incomplete_data& ec) {
            // 不完整数据
            // 少了 ec.less() 字节
            // TODO
            std::cout << "incomplete_data" << std::endl;
            // 临时方案
            //delete_this();
            this->close();
        }
        catch (EncryptException& ec) {
            std::cout << ec.what() << std::endl; 
            std::cout << __LINE__ << " Error right_read_handler, close.\n";
            //delete_this(); 
            this->close();
        }
        catch (DecryptException& ec) {
            std::cout << ec.what() << std::endl; 
            std::cout << __LINE__ << " Error right_read_handler, close.\n";
            //delete_this(); 
            this->close();
        }
        catch (...) {
            // TODO
            // 临时方案
            std::cout << "error [local_session.cpp:read_right_handler]\n";
            //delete_this();
            this->close();
        }
        //break;}
    }
    else {
        // <debug>
        std::cout << "close this " << __LINE__ << '\n';
        // </debug>
        //delete_this();
        this->close();
    }
}

void session::transport(void) {
    // 第一次 用 data_left 读数据
    this->data_left.resize(max_length, 0);
    this->socket_left.async_read_some(
            boost::asio::buffer(this->data_left, max_length),
            boost::bind(&session::left_read_handler, 
                shared_from_this(), _1, _2));
                //boost::asio::placeholders::error,
                //boost::asio::placeholders::bytes_transferred));

    // 每次异步读数据前，清空 data
    //this->lss_reply.assign_data(max_length, 0);
    auto&& lss_reply = make_shared_reply();
    this->socket_right.async_read_some(lss_reply->buffers(), 
            boost::bind(&session::right_read_handler, 
                shared_from_this(), _1, _2, lss_reply));
}

void session::left_read_handler(const boost::system::error_code& error,
        size_t bytes_transferred) {
    std::cout << "left_read_handler\n";
    std::cout << "---> bytes_transferred = " 
        << std::dec << bytes_transferred << "\n";

    if (! error) {

        std::cout << "read data from client:";
        _debug_print_data(this->data_left, char(), 0);
        _debug_print_data(this->data_left, int(), ' ', std::hex);

        // 封包
        //auto&& rqst = pack_data(bytes_transferred);
        // 发送至服务端
        auto&& data_request = make_shared_request(pack_data(bytes_transferred));
        boost::asio::async_write(socket_right, 
                data_request->buffers(),
                boost::bind(&session::right_write_handler, 
                    shared_from_this(), _1, _2, data_request));
                    //boost::asio::placeholders::error,
                    //boost::asio::placeholders::bytes_transferred)); 
        // debug
        std::cout << "and send to server." << std::endl;
    }
    else {
        std::cout << "close this " << __LINE__ << '\n';
        //delete_this();
        this->close();
    }
}

void session::right_write_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_request_type lss_request) {
    (void)lss_request;
    // <debug>
    std::cout << "right_write_handler\n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        // 每次异步读数据前，清空 data
        this->data_left.assign(max_length, 0);
        socket_left.async_read_some(
                boost::asio::buffer(this->data_left, max_length),
                boost::bind(&session::left_read_handler, 
                    shared_from_this(), _1, _2));
    }
    else {
        // <debug>
        std::cout << "close this " << __LINE__ << '\n';
        // </debug>
        //delete_this();
        this->close();
    }
}

void session::left_write_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "left_write_handler\n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        /*
        this->data_left.assign(max_length, 0);
        socket_right.async_read_some(boost::asio::buffer(data_left, max_length),
                boost::bind(&session::right_read_handler, this, _1, _2));
        */
        //this->lss_reply.assign_data(max_length, 0);
        auto&& lss_reply = make_shared_reply();
        this->socket_right.async_read_some(lss_reply->buffers(),
                boost::bind(&session::right_read_handler, shared_from_this(), 
                    _1, _2, lss_reply));
    }
    else {
        // <debug>
        std::cout << "close this " << __LINE__ << '\n';
        // </debug>
        //delete_this();
        this->close();
    }
}

// 组装 hello
const request& session::pack_hello(void) {
    static const request hello(0x00, request::hello, 0, data_t());
    return hello;
    //return this->lss_request.assign(0x00, request::hello, 0, data_t());
};
// 组装 exchange
const request session::pack_exchange(const keysize_t& keysize, 
        const data_t& public_key) {
    // step 1 生成随机数
    this->random_str = lproxy::random_string::generate_number();
    // step 2 认证 key
    const sdata_t& auth_key = config::get_instance().get_auth_key();
    crypto::Encryptor md5(new crypto::Md5());
    sdata_t md5_auth_key;
    md5.encrypt(md5_auth_key, &auth_key[0], auth_key.size());
    // step 3 组装 data
    sdata_t&& data_ = md5_auth_key + this->random_str;
    //data_t data(data_.begin(), data_.end());

    // 用 keysize 和 public_key 构造 rsa 加密器
    crypto::Encryptor rsa_encryptor(
            new crypto::Rsa(
                    (crypto::RsaKey::size)keysize, 
                    &public_key[0], public_key.size()
                ) // crypto::Rsa
            ); // crypto::Encryptor
    // 对数据加密
    vdata_t cipher;
    rsa_encryptor.encrypt(cipher, (const_byte_ptr)&data_[0], data_.size());

    // step 4 组装 lproxy::local::request
    //return request(0x00, request::exchange, cipher.size(), cipher);
    /*
    return this->lss_request.assign(0x00, request::exchange, cipher.size(), 
            data_t(cipher.begin(), cipher.end()));
    */
    return request(0x00, request::exchange, cipher.size(),
            data_t(cipher.begin(), cipher.end()));
}
// 组装 data
const request session::pack_data(std::size_t data_len) {
    // 用 密文this->data_key 构造 this->aes_encryptor 加密器
    if (! this->aes_encryptor) {
        this->aes_encryptor = std::make_shared<crypto::Encryptor>(
                new crypto::Aes(this->data_key, crypto::Aes::raw256keysetting()));
    }
    // 加密
    vdata_t data;
    this->aes_encryptor->encrypt(data, &(this->data_left[0]), data_len);

    std::cout << "packing data: \nthis->data_key = ";
    _debug_print_data(this->data_key, int(), ' ', std::hex);

    if (config::get_instance().get_zip_on()) {
        // TODO
        // 压缩 data

        //return request(0x00, request::zipdata, data_len, 
        //    data_t(data.begin(), data.end()));
        /*
        return this->lss_request.assign(0x00, request::zipdata, data_len,
                data_t(data.begin(), data.end()));
        */
        return request(0x00, request::zipdata, data_len,
                data_t(data.begin(), data.end()));
    }

    // debug
    std::cout << "pack data/zipdata: ";
    _debug_print_data(
            get_vdata_from_lss_pack(
                request(0x00, request::data, data_len,
                    data_t(data.begin(), data.end()))), 
                int(), ' ', std::hex);

    //return request(0x00, request::data, data_len, 
    //        data_t(data.begin(), data.end()));
    /*
    return this->lss_request.assign(0x00, request::data, data_len,
            data_t(data.begin(), data.end()));
    */
    return request(0x00, request::data, data_len,
            data_t(data.begin(), data.end()));
}

// 组装 bad
const request& session::pack_bad(void) {
    static const request bad(0x00, request::bad, 0x00, data_t());
    return bad;
    //return this->lss_request.assign(0x00, request::bad, 0x00, data_t());
}



void session::unpack_reply_hello(keysize_t& keysize, data_t& public_key,
        const reply& reply) {
    const data_t&     data     = reply.get_data();
    //const std::size_t size     = sizeof (keysize_t);
    const std::size_t data_len = reply.data_len();
    keysize = data[0];
    keysize = ((keysize << 8) & 0xff00) | data[1];
    //public_key.assign(&data[size], &data[data_len]);
    public_key.assign(data.begin() + 2, data.begin() + data_len);
}



// 获取 随机key 和 随机数
void session::unpack_reply_exchange(sdata_t& reply_random_str, 
        const reply& reply) {
    const data_t&     data     = reply.get_data(); // 当前是密文
    const std::size_t data_len = reply.data_len();
    // 用this->auth_key 构造aes 加解密器
    const sdata_t& auth_key = config::get_instance().get_auth_key();
    auto&& md5 = crypto::Encryptor(new crypto::Md5());
    sdata_t cipher_auth_key;
    md5.encrypt(cipher_auth_key, (const_byte_ptr)&auth_key[0], auth_key.size());
    auto&& aes = std::make_shared<crypto::Encryptor>(
            new crypto::Aes(cipher_auth_key, crypto::Aes::raw256keysetting()));
    vdata_t plaintext;
    // 解密data
    aes->decrypt(plaintext, &data[0], data_len);

    // 随机key赋值， 得到密文的 data_key
    this->data_key.assign(plaintext.begin(), plaintext.begin() + 32);

    // 获取 server 端发来的随机数
    reply_random_str.assign(plaintext.begin() + 32, plaintext.end());

    // debug
    std::cout << "get data_key: " << std::endl;
    _debug_print_data(this->data_key, int(), ' ', std::hex);
    std::cout << "get reply_random_str:" << std::endl;
    _debug_print_data(reply_random_str, char(), 0);
    _debug_print_data(reply_random_str, int(), ' ', std::hex);
    std::cout << "this->random_str = " << this->random_str << std::endl;
}


/**
 * @brief unpack_reply_data
 * @param data_right [out]
 * @param lss_length  当前尚未解包的 lss_reply 数据长度, 
 *                      总是传入 bytes_trannsferred
 * @param reply      [lproxy::local::reply] lss_reply
 * @param is_zip [bool, default false]
 * @return [const int]     这次解包后（unpack 执行完），还剩
 *                          未处理的 lss_reply 数据长度. 
 *      = 0 数据已处理完毕
 *      > 0 lss_reply 中还有未处理完的数据
 *      < 0 当前 lss_reply 数据包不完整
 */
const int session::unpack_reply_data(data_t& data_right, std::size_t lss_length,
        const reply& reply, bool is_zip/*=false*/) {
    const data_t&     data     = reply.get_data(); // 当前是密文
    const std::size_t data_len = reply.data_len();
    
    if (is_zip) {
        // TODO
        // 解压 data
    }
    if (! (this->aes_encryptor)) {
        this->aes_encryptor = std::make_shared<crypto::Encryptor>(
            new crypto::Aes(this->data_key, crypto::Aes::raw256keysetting()));
    
    }
    vdata_t data_right_;
    this->aes_encryptor->decrypt(data_right_, &data[0], data_len);
    data_right.assign(data_right_.begin(), data_right_.end());

    return lss_length - (4 + data_len); 
    // (4 + data_len) 当前已经处理的lss包长度
}



/*
http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/socks4/socks4.hpp
http://www.boost.org/doc/libs/1_49_0/doc/html/boost_asio/example/socks4/sync_client.cpp
http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/example/cpp11/echo/async_tcp_echo_server.cpp
http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/example/cpp11/echo/blocking_tcp_echo_client.cpp
http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/example/cpp03/echo/async_tcp_echo_server.cpp
 */
