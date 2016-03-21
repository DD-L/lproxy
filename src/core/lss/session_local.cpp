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
    loginfo("client: " << socket_left.remote_endpoint().address());
    // server
    const auto& server_name = config::get_instance().get_server_name();
    const auto& server_port = config::get_instance().get_server_port();

    lsslogdebug("async_resolve: " << server_name << ":" << server_port);

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
        // 主机不可达
        logerror(ec.message() << " Host unreachable. close this");
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
        
        lsslogdebug("send hello to server: "
            << _debug_format_data(get_vdata_from_lss_pack(pack_hello()), int(),
                ' ', std::hex));
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
        // 网络不可达
        logerror(ec.message() << " Network unreachable. close this");
        this->close();
    }
}


void session::hello_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    lsslogdebug("---> bytes_transferred = " << std::dec << bytes_transferred);
    if (! error) {

        auto&& lss_reply = make_shared_reply();

        this->socket_right.async_read_some(lss_reply->buffers(), 
                boost::bind(&session::right_read_handler, shared_from_this(),
                    _1, _2, lss_reply, lproxy::placeholders::shared_data,
                    lproxy::placeholders::shared_data));
                    //boost::asio::placeholders::error,
                    //boost::asio::placeholders::bytes_transferred));
        this->status = status_hello;
    }
    else {
        logerror(error.message() << " close this");
        this->close();
    }
}

void session::exchange_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_request_type lss_request) {
    (void)lss_request;
    lsslogdebug("---> bytes_transferred = " << std::dec << bytes_transferred);
    if (! error) {
        auto&& lss_reply = make_shared_reply();
        this->socket_right.async_read_some(lss_reply->buffers(), 
                boost::bind(&session::right_read_handler, shared_from_this(), 
                    _1, _2, lss_reply, lproxy::placeholders::shared_data,
                    lproxy::placeholders::shared_data));
        status = status_auth;
    }
    else {
        logerror(error.message() << " close this");
        this->close();
    }
}

void session::right_read_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_reply_type lss_reply, 
        shared_data_type __data_right_rest, shared_data_type __write_data) {
    (void)__write_data;
    if (__data_right_rest->size() > 0) {
        // 先修正 bytes_transferred
        // bytes_transferred += 上一次的bytes_transferred
        bytes_transferred += lss_reply->get_data().size() + 4; // lss 包头长度为4
        // 分包处理后，遗留的数据
        // append 追加数据__data_right_rest
        // 修正 lss_reply
        lss_reply->get_data().insert(lss_reply->get_data().end(), 
                __data_right_rest->begin(), __data_right_rest->end());
        lsslogdebug("lss_reply - rectified");

    }
    lsslogdebug("---> bytes_transferred = " << std::dec << bytes_transferred);

    lsslogdebug("lss_reply->version() = " << (int)lss_reply->version());
    lsslogdebug("lss_reply->type() = " << (int)lss_reply->type());
    lsslogdebug("lss_reply->data_len() = " << lss_reply->data_len());
    lsslogdebug(_debug_format_data(get_vdata_from_lss_pack(*lss_reply),
            int(), ' ', std::hex));

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
                assert_status(status_hello);

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

                lsslogdebug("send exchange to server: "
                    << _debug_format_data(get_vdata_from_lss_pack(
                            *exchange_request), int(), ' ', std::hex));
                break;
            } 
            case reply::exchange: { // 0x03
                assert_status(status_auth);

                sdata_t reply_random_str;
                // 解包，获取 随机数 和 随机key
                unpack_reply_exchange(reply_random_str, *lss_reply);
                // 验证 随机 数
                if (reply_random_str != this->random_str) {
                    // 非法的server端
                    logwarn("Illegal server !!! close this.");
                    this->close();
                    break; 
                }

                // 通过验证
                lsslogdebug("Verify completed");

                transport(); 

                status = status_data;
                break;
            }
            case reply::deny: // 0x04
                    // 被 server 端拒绝
                    logwarn("Connection rejected by the server-side. "
                            "close this.");
                    this->close();
                break;
            case reply::timeout: // 0x05
                    // server端session超时;
                    logwarn("Server-side session timeout. close this.");
                    this->close();
                break;
            case reply::zipdata:// 0x17
                is_zip_data = true;
            case reply::data: { // 0x06
                assert_status(status_data);
                // 解包
                data_t data_right;
                const int rest_lss_data_len = 
                    unpack_reply_data(data_right, bytes_transferred, 
                            *lss_reply, is_zip_data);

                if (rest_lss_data_len < 0) {
                    throw incomplete_data(0 - rest_lss_data_len); 
                }

                auto&& data = lproxy::make_shared_data(
                        std::move(data_right));

                // 裁剪 lss_reply 数据，（当前数据已缓存到*data）
                // 减掉当前lss数据。 (分包)
                bool is_continue = 
                    cut_lss(bytes_transferred - rest_lss_data_len,
                        bytes_transferred, *lss_reply);
                // bytes_transferred - rest_lss_data_len 的意义是 
                // 新包在 旧包中 开始的位置（0开头）


                // 将 data_right 发至 client
                if (rest_lss_data_len > 0 && is_continue) {
                    // lss_reply 里还有未处理的数据
                    lsslogdebug("unprocessed data still in lss_reply..");

                    boost::asio::async_write(this->socket_left,
                        boost::asio::buffer(*data),
                        boost::bind(&session::right_read_handler, 
                            shared_from_this(),
                            boost::asio::placeholders::error,
                            std::size_t(rest_lss_data_len), 
                            lss_reply, lproxy::placeholders::shared_data,
                            data));
                }
                else {
                    // 最后一条不可再分割的数据再绑定 left_write_handler
                    boost::asio::async_write(this->socket_left,
                         boost::asio::buffer(*data),
                         boost::bind(&session::left_write_handler,
                           shared_from_this(), _1, _2, data)); 
                }

                lsslogdebug("send plain data to client : "
                        << _debug_format_data(*data, int(), ' ', std::hex)
                        //<< std::endl
                        //<< _debug_format_data(*data, char(), 0)
                        );
                break;
            }
            case reply::bad: // 0xff
            default:
                    // 数据包 bad
                    logwarn("lss packet is bad. close this");
                    this->close();
                break;
            }
        }
        catch (wrong_packet_type&) {
            // default:
            // TODO
            // 临时方案
            logerror("wrong_packet_type. close this");
            this->close();
        }
        catch (incomplete_data& ec) {
            // 不完整数据
            // 少了 ec.less() 字节
            // 有可能是右边的数据，分包遗留的未读数据
            logwarn("incomplete_data. ec.less() = " << ec.less() << " byte.");
            if (ec.less() > 0) {
                //  
                auto&& data_right_rest = lproxy::make_shared_data(
                        (std::size_t)ec.less(), 0);

                lsslogdebug("incomplete_data. start to async-read " 
                        << ec.less() << " byte data from socket_right");
                this->socket_right.async_read_some(
                        boost::asio::buffer(&(*data_right_rest)[0], 
                            (std::size_t)ec.less()), 
                        boost::bind(&session::right_read_handler, 
                            shared_from_this(), 
                            _1, _2, lss_reply, data_right_rest,
                            lproxy::placeholders::shared_data));
            }
            else {
                logwarn("close this");
                this->close();
            }
        }
        catch (wrong_lss_status& ec) {
            logerror(ec.what() << ". close this");
            this->close();
        }
        catch (EncryptException& ec) {
            logerror(ec.what() << ". close this");
            this->close();
        }
        catch (DecryptException& ec) {
            logerror(ec.what() << ". close this");
            this->close();
        }
        catch (std::exception& ec) {
            logerror(ec.what() << ". close this");
            this->close();
        }
        catch (...) {
            logerror("close this");
            this->close();
        }
        //break;}
    }
    else {
        logerror(error.message() << " close this");
        this->close();
    }
}

void session::transport(void) {
    auto&& data_left = lproxy::make_shared_data(max_length, 0);
    this->socket_left.async_read_some(
            boost::asio::buffer(&(*data_left)[0], max_length),
            boost::bind(&session::left_read_handler, 
                shared_from_this(), _1, _2, data_left));
                //boost::asio::placeholders::error,
                //boost::asio::placeholders::bytes_transferred));

    auto&& lss_reply = make_shared_reply();
    this->socket_right.async_read_some(lss_reply->buffers(), 
            boost::bind(&session::right_read_handler, 
                shared_from_this(), _1, _2, 
                lss_reply, lproxy::placeholders::shared_data,
                lproxy::placeholders::shared_data));
}

void session::left_read_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_data_type data_left) {
    lsslogdebug("---> bytes_transferred = " << std::dec << bytes_transferred);

    if (! error) {

        lsslogdebug("read data from client: " 
                //<< _debug_format_data(*data_left, char(), 0)
                //<< std::endl
                << _debug_format_data(*data_left, int(), ' ', std::hex));

        // 封包
        // 发送至服务端
        auto&& data_request = make_shared_request(
                pack_data(*data_left, bytes_transferred));
        boost::asio::async_write(socket_right, 
                data_request->buffers(),
                boost::bind(&session::right_write_handler, 
                    shared_from_this(), _1, _2, data_request));
                    //boost::asio::placeholders::error,
                    //boost::asio::placeholders::bytes_transferred)); 
        // debug
        lsslogdebug("and send to server.");
    }
    else {
        if (error == boost::asio::error::eof) {
            // for HTTP Connection: Keep-Alive
            logerror(error.message() << " value = " << error.value() 
                    << ". Restart async-read-left with timeout");
            auto&& data_left = lproxy::make_shared_data(max_length, 0);
            this->socket_left.async_read_some(
                    boost::asio::buffer(&(*data_left)[0], max_length),
                    boost::bind(&session::left_read_handler, 
                        shared_from_this(), _1, _2, data_left));
            deadline_timer t(this->socket_left.get_io_service(),
                    boost::posix_time::seconds(
                        config::get_instance().get_timeout()));
            t.async_wait(boost::bind(
                        &session::left_read_timeout_handler,
                        shared_from_this(), _1));
        }
        else {
            logerror(error.message() << " value = " << error.value()
                    << " close this");
            this->close();
        }
    }
}


void session::left_read_timeout_handler(const boost::system::error_code& error) {
    logerror(error.message() << " value = " << error.value() 
            << " close this");
    /*
    if (error != boost::asio::error::operation_aborted) {
    }
    else {
    }
    */
    this->close();
}

void session::right_write_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_request_type lss_request) {
    (void)lss_request;
    lsslogdebug("---> bytes_transferred = " << std::dec << bytes_transferred);
    if (! error) {
        auto&& data_left = lproxy::make_shared_data(max_length, 0);
        socket_left.async_read_some(
                boost::asio::buffer(&(*data_left)[0], max_length),
                boost::bind(&session::left_read_handler, 
                    shared_from_this(), _1, _2, data_left));
    }
    else {
        logerror(error.message() << " close this");
        this->close();
    }
}

void session::left_write_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred, shared_data_type __data) {
    (void)__data;
    lsslogdebug("---> bytes_transferred = " << std::dec << bytes_transferred);
    if (! error) {
        auto&& lss_reply = make_shared_reply();
        this->socket_right.async_read_some(lss_reply->buffers(),
                boost::bind(&session::right_read_handler, shared_from_this(), 
                    _1, _2, lss_reply, lproxy::placeholders::shared_data,
                    lproxy::placeholders::shared_data));
    }
    else {
        logerror(error.message() << " close this");
        this->close();
    }
}

// 组装 hello
const request& session::pack_hello(void) {
    static const request hello(0x00, request::hello, 0, data_t());
    return hello;
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
    return request(0x00, request::exchange, cipher.size(),
            data_t(cipher.begin(), cipher.end()));
}
// 组装 data
const request session::pack_data(const data_t& data_left, 
        const std::size_t data_len) {
    // 用 密文this->data_key 构造 this->aes_encryptor 加密器
    if (! this->aes_encryptor) {
        this->aes_encryptor = std::make_shared<crypto::Encryptor>(
              new crypto::Aes(this->data_key, crypto::Aes::raw256keysetting()));
    }
    // 加密
    vdata_t data;
    this->aes_encryptor->encrypt(data, &data_left[0], data_len);

    if (config::get_instance().get_zip_on()) {
        // TODO
        // 压缩 data

        return request(0x00, request::zipdata, data_len,
                data_t(data.begin(), data.end()));
    }

    auto&& request_data = request(0x00, request::data, data_len,
            data_t(data.begin(), data.end()));

    lsslogdebug("pack data/zipdata: " <<
            _debug_format_data(get_vdata_from_lss_pack(request_data), 
                    int(), ' ', std::hex));

    return request_data;
}

// 组装 bad
const request& session::pack_bad(void) {
    static const request bad(0x00, request::bad, 0x00, data_t());
    return bad;
}



void session::unpack_reply_hello(keysize_t& keysize, data_t& public_key,
        const reply& reply) {
    const data_t&     data     = reply.get_data();
    const std::size_t data_len = reply.data_len();
    keysize = data[0];
    keysize = ((keysize << 8) & 0xff00) | data[1];
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
