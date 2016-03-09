/*************************************************************************
	> File Name:    session_server.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/1 4:29:50
 ************************************************************************/

#include <lss/session_server.h>
#include <lss/config.h>
#include <crypto/aes_crypto.h>

using namespace lproxy::server;
using lproxy::tcp;
using lproxy::udp;
using lproxy::data_t;

session::session(boost::asio::io_service& io_service_left,
                  boost::asio::io_service& io_service_right)
            : socket_left(io_service_left), socket_right_tcp(io_service_right),
            socket_right_udp(io_service_right)
            //ios_right(io_service_right),
            /*resolver_right(io_service_right), */
{
    this->status = status_not_connected;
    this->socks5_state = lproxy::socks5::server::OPENING;            
}

void session::start(void) {
    std::cout << "client: ";
    std::cout << socket_left.remote_endpoint().address() << std::endl;      

    status = status_connected;

    std::cout << "start read msg from local.." << std::endl;
    // 第一次 用 this->lss_request 读数据, 需要开辟空间
    this->lss_request.set_data_size(max_length);            

    socket_left.async_read_some(this->lss_request.buffers(),
            boost::bind(&session::left_read_handler, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred));
}
tcp::socket& session::get_socket_left(void) {
    return this->socket_left; 
}

void session::delete_this(void) {
    if (! delete_flag.test_and_set()) {
        // TODO
        // step 1
        // cancel session 上所有的异步
        // http://www.boost.org/doc/libs/1_59_0/doc/html/boost_asio/reference/basic_stream_socket/cancel/overload1.html
        socket_left.close();
        socket_right_tcp.close();
        socket_right_udp.close();
        // step 2
        delete this;
    }
}

void session::left_read_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "left_read_handler \n<--- bytes_transferred = "
        << std::dec << bytes_transferred << '\n'; 
    // </debug>
    if (! error) {
    //switch (this->lss_request.version()) {
    //case 0x00:}
        try {

            // lss包完整性检查
            lss_pack_integrity_check(bytes_transferred, this->lss_request);

            switch (this->lss_request.type()) {
            case request::hello: { // 0x00
                // 验证 hello 包是否正确
                if (this->lss_request.data_len()) {
                    throw wrong_packet_type();
                }
                // 把模长和公钥 打包发送给 local
                // 组装明文 data
                auto& rply_hello = pack_hello();
                // 发送给 local
                boost::asio::async_write(socket_left, rply_hello.buffers(),
                        boost::bind(&session::hello_handler, this, _1, _2));
                            //boost::asio::placeholders::error,
                            //boost::asio::placeholders::bytes_transferred));
                break;
            }
            case request::exchange: { // 0x02
                // step 0. 判断状态是否为 status_hello
                // step 1. 解包，得到 密文的auth_key 和 随机数
                // step 2. 验证 auth_key
                // step 2.1 如果验证失败,发送 0x00, 0x04, 0x00, 0x00,delete_this
                // step 2.2 如果验证通过
                //          1. 生成随机 key (data_key)
                //          2. 打包 随机key + 随机数
                //          3. 将打包后的数据 用 auth_key 进行aes加密。
                //          4. 将加密后的数据发送至local, 状态设置为 status_auth
                //          5. 在write handler里将状态设置为 status_data
                if (status_hello != status) {
                    throw wrong_packet_type();
                }

                data_t auth_key, random_str;
                unpack_request_exchange(auth_key, random_str);

                // 验证 auth_key
                const auto& key_set 
                    = config::get_instance().get_cipher_auth_key_set();
                if (key_set.find(auth_key) == key_set.end()) {
                    // 将打包好的"认证失败"数据 发送至 local, 然后再delete_this
                    auto& rply_deny = pack_deny();
                    boost::asio::async_write(socket_left, rply_deny.buffers(),
                            boost::bind(&session::delete_this, this));
                    break;
                }
                else {
                    // 验证通过
                    // 组装 reply::exchange 发给 local
                    auto&& rply_exchange = pack_exchange(auth_key, random_str);
                    boost::asio::async_write(this->socket_left, 
                            rply_exchange.buffers(),
                            boost::bind(&session::exchange_handler, this,
                                boost::asio::placeholders::error,
                                boost::asio::placeholders::bytes_transferred));
                    status = status_auth;
                    break;
                }
            }
            case request::zipdata: // 0x17
            case request::data: { // 0x06
                if (status_data != status) {
                    throw wrong_packet_type();
                }
                // step 1
                //  解包 得到 plain_data , plain_data 通常是 socks5 数据 
                //std::string data;
                //get_plain_data(data);

                data_t plain_data;
                unpack_data(plain_data);

                // step 2
                //  将 plain_data 交付给 socks5 处理
                switch (this->socks5_state) {
                case lproxy::socks5::server::OPENING: {
                    // 用 plain_data 得到 package
                    lproxy::socks5::ident_req ir(&plain_data[0], 
                            plain_data.size());
                    data_t package;
                    lproxy::socks5::ident_resp::pack(package, &ir);
                    // 将 package 加密封包
                    auto&& rply_data = pack_data(package, package.size());
                    // 发给 local
                    boost::asio::async_write(socket_left, 
                            rply_data.buffers(),
                            boost::bind(&session::left_read_handler, this,
                               boost::asio::placeholders::error,
                               boost::asio::placeholders::bytes_transferred));

                    this->socks5_state = lproxy::socks5::server::CONNECTING; 
                    break;
                }
                case lproxy::socks5::server::CONNECTING: {
                    // 用 plain_data 得到 package
                    lproxy::socks5::req rq(&plain_data[0], plain_data.size());

                    // 分析 rq , 该干啥干啥...
                    socks5_request_processing(rq);
                    
                    break;
                }
                case lproxy::socks5::server::CONNECTED:
                    switch (this->socks5_cmd) {
                    case CMD_CONNECT:
                        boost::asio::async_write(this->socket_right_tcp, 
                            boost::asio::buffer(&plain_data[0], 
                                 plain_data.size()),
                            boost::bind(&session::right_write_handler, this,
                                 boost::asio::placeholders::error,
                                 boost::asio::placeholders::bytes_transferred));
                        break;
                    case CMD_BIND:
                        // TODO
                        // 临时方案：
                        //
                        boost::asio::async_write(this->socket_left, 
                                pack_bad().buffers(),
                                boost::bind(&session::delete_this, this));
                        break;
                    case CMD_UDP: {
                        ip::udp::endpoint destination(
                             ip::address::from_string(this->dest_name), 
                             this->dest_port);
                        socket_right_udp.async_send_to(
                             boost::asio::buffer(&plain_data[0], 
                                 plain_data.size()), destination,
                             boost::bind(&session::right_write_handler, this,
                                 boost::asio::placeholders::error,
                                 boost::asio::placeholders::bytes_transferred));
                        break;
                    }
                    default: {
                        // 发送给 local 让其关闭它的session, lss_pack_type::0xff
                        //auto& rply_data = pack_bad();
                        boost::asio::async_write(socket_left, 
                                pack_bad().buffers(),
                                boost::bind(&session::delete_this, this));
                    }
                    } // switch (this->socks5_cmd)
                    break;
                default:
                    break;
                } // switch (this->socks5_state)

                break;
            }
            case request::bad: // 0xff
            default:
                std::cout << "数据包 bad\n";
                delete_this();
                break;
            } // switch (this->lss_request.type())
        }
        catch (wrong_packet_type&) {
            // 临时解决方案
            boost::asio::async_write(socket_left, 
                    pack_bad().buffers(),
                    boost::bind(&session::delete_this, this));
        }
        catch (incomplete_data& ec) {
            // 不完整数据
            // 少了 ec.less() 字节
            // 临时解决方案
            boost::asio::async_write(socket_left, 
                    pack_bad().buffers(),
                    boost::bind(&session::delete_this, this));
        }
        catch (lproxy::socks5::illegal_data_type&) { // 非法的socks5数据
            // delete_this 
            // 临时解决方案
            boost::asio::async_write(socket_left, 
                    pack_bad().buffers(),
                    boost::bind(&session::delete_this, this));
        }
        catch (lproxy::socks5::unsupported_version&) { // 不支持的 socks5 版本
            // deny
            // 临时解决方案
            boost::asio::async_write(socket_left, 
                    pack_bad().buffers(),
                    boost::bind(&session::delete_this, this));
        }
        catch (...) {
            delete_this(); 
        }
    } 
    else { // error
        delete_this(); 
    }
}


void session::hello_handler(const boost::system::error_code& error,
            std::size_t bytes_transferred) {
    // <debug>
    std::cout << "hello_handler \n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        status = status_hello;
        socket_left.async_read_some(this->lss_request.buffers(), 
                boost::bind(&session::left_read_handler, this,
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
        socket_left.async_read_some(this->lss_request.buffers(), 
                boost::bind(&session::left_read_handler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
        status = status_data;
    }
    else {
        // <debug>
        std::cout << "delete this " << __LINE__ << '\n';
        // </debug>
        delete_this();
    }

}


//left_write_handler 【leftread -> left_read_handler】
void session::left_write_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "left_write_handler \n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        socket_left.async_read_some(this->lss_request.buffers(), 
                boost::bind(&session::left_read_handler, this, _1, _2));
                    //boost::asio::placeholders::error,
                    //boost::asio::placeholders::bytes_transferred));
    }
    else {
        // <debug>
        std::cout << "delete this " << __LINE__ << '\n';
        // </debug>
        delete_this();
    }
} 

//void right_write_handler 【rightread -> right_read_handler】
void session::right_write_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "right_write_handler \n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        this->data_right.resize(max_length, 0);
        switch (this->socks5_cmd) {
        case CMD_CONNECT:
            socket_right_tcp.async_read_some(
                    boost::asio::buffer(&this->data_right[0], max_length), 
                    boost::bind(&session::right_read_handler, this, _1, _2));
                        //boost::asio::placeholders::error,
                        //boost::asio::placeholders::bytes_transferred));
            break;
        case CMD_UDP: {
            ip::udp::endpoint destination(
                ip::address::from_string(this->dest_name), this->dest_port);
            socket_right_udp.async_receive_from(
                boost::asio::buffer(&this->data_right[0], max_length), 
                destination,
                boost::bind(&session::right_read_handler, this, _1, _2));
                    //boost::asio::placeholders::error,
                    //boost::asio::placeholders::bytes_transferred));
            break;
        }
        case CMD_BIND:
            /* TODO */
        case CMD_UNSUPPORT:
        default:
            boost::asio::async_write(socket_left, pack_bad().buffers(),
                    boost::bind(&session::delete_this, this));
        } // switch (this->socks5_cmd)
    }
    else {
        // <debug>
        std::cout << "delete this " << __LINE__ << '\n';
        // </debug>
        boost::asio::async_write(socket_left, 
                pack_bad().buffers(),
                boost::bind(&session::delete_this, this));
    }
}
//void right_read_handler  【leftwrite -> left_write_handler】
void session::right_read_handler(const boost::system::error_code& error,
        std::size_t bytes_transferred) {
    // <debug>
    std::cout << "right_read_handler \n---> bytes_transferred = "
        << std::dec << bytes_transferred << '\n';
    // </debug>
    if (! error) {
        // step 1, 修正 data_right 的大小
        //this->data_right.resize(bytes_transferred);
        // step 2, 将读来的数据，加密打包
        auto&& data = pack_data(this->data_right, bytes_transferred);
        // step 3, 发给 local 端
        boost::asio::async_write(socket_left, data.buffers(),
                boost::bind(&session::left_write_handler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    }
    else {
        // <debug>
        std::cout << "delete this " << __LINE__ << '\n';
        // </debug>
        boost::asio::async_write(socket_left, 
                pack_bad().buffers(),
                boost::bind(&session::delete_this, this));
    }
}


const reply& session::pack_hello(void) {
    static const data_t&& data = gen_hello_data();
    // 明文
    static const reply hello(0x00, reply::hello, data.size(), data);
    return hello;
}

const reply& session::pack_bad(void) {
    static const reply bad(0x00, reply::bad, 0x00, data_t());
    return bad;
}

const reply& session::pack_timeout(void) {
    static const reply timeout(0x00, reply::timeout, 0x00, data_t());
    return timeout;
}

const reply& session::pack_deny(void) {
    static const reply deny(0x00, reply::deny, 0x00, data_t()); 
    return deny;
}


const reply session::pack_exchange(const data_t& auth_key,
        const data_t& random_str) {

    assert(auth_key.size() == 32);

    // 1. 生成随机 key (密文的data_key)
    vdata_t&& data_key = lproxy::random_string::generate(32);
    assert(data_key.size() == 32);

    this->aes_encryptor = std::make_shared<crypto::Encryptor>(
            new crypto::Aes(data_key, crypto::Aes::raw256keysetting()));

    // 2. 打包 随机key + 随机数
    data_t plain = &data_key[0] + random_str;
    vdata_t cipher;
    // 3. 将打包后的数据 用 auth_key 进行aes加密。
    crypto::Encryptor aescryptor(new crypto::Aes(
                (const char*)&auth_key[0], crypto::Aes::raw256keysetting()));
    aescryptor.encrypt(cipher, &plain[0], plain.size());
    
    data_t cipher_(cipher.begin(), cipher.end());
    return reply(0x00, reply::exchange, cipher_.size(), cipher_);
}

const reply session::pack_data(const std::string& data, std::size_t data_len) {
    data_t data_(data.begin(), data.begin() + data_len);
    return pack_data(data_, data_len);
}
const reply session::pack_data(const data_t& data, std::size_t data_len) {
    if (! this->aes_encryptor) {
        // this->aes_encrytor 未被赋值
        std::cout << "this->aes_encrytor 未被赋值" << std::endl;

        //发送 deny, 并 delete_this
        auto& deny = pack_deny();
        boost::asio::async_write(socket_left, deny.buffers(),
                boost::bind(&session::delete_this, this));
        return deny;
    }
    else {
        // 对包加密
        vdata_t cipher;
        this->aes_encryptor->encrypt(cipher, &data[0], data_len);
        data_t data_(cipher.begin(), cipher.end());
        
        // 压缩数据
        if (config::get_instance().get_zip_on()) {
            // TODO 
        }

        // 封包
        return reply(0x00, reply::data, data_.size(), data_);
    }
}

const data_t session::gen_hello_data() {
    auto& config  = config::get_instance();
    auto&& keysize = config.get_rsa_keysize();
    auto& publickey = config.get_rsa_publickey_hex();
    byte keysize_arr[2] = {0};
    keysize_arr[0] = (keysize >> 8) & 0xff; // high_byte
    keysize_arr[1] = keysize & 0xff;        // low_byte
    sdata_t data(keysize_arr, keysize_arr+2); 
    data += publickey;
    return data_t(data.begin(), data.end());
}

void session::unpack_request_exchange(data_t& auth_key, data_t& random_str) {
    // 对包解密
    crypto::Encryptor rsa_encryptor(
            new crypto::Rsa(config::get_instance().get_rsakey()));
    auto& cipher = this->lss_request.get_data();
    std::size_t cipher_len = this->lss_request.data_len();
    vdata_t plain;
    rsa_encryptor.decrypt(plain, &cipher[0], cipher_len);

    // 取 plain 的前 256bit (32byte) 为 auth_key, 余下的为 随机字符串
    auth_key.assign(plain.begin(), plain.begin() + 32);
    random_str.assign(plain.begin() + 32, plain.end());
}

data_t& session::unpack_data(data_t& plain) {
    if (! this->aes_encryptor) {
        // this->aes_encrytor 未被赋值
        std::cout << "this->aes_encrytor 未被赋值" << std::endl;

        //发送 deny, 并 delete_this
        auto& deny = pack_deny();
        boost::asio::async_write(socket_left, deny.buffers(),
                boost::bind(&session::delete_this, this));
    }
    else {
        // 解压数据
        if (config::get_instance().get_zip_on()) {
            // TODO 
        }
        // 对包解密
        data_t&         cipher = this->lss_request.get_data();
        std::size_t cipher_len = this->lss_request.data_len();
        vdata_t plain_;
        aes_encryptor->decrypt(plain_, cipher.c_str(), cipher_len);
        plain.assign(plain_.begin(), plain_.end());

        // data 清零, 有必要? TODO
        //cipher = data_t();
    }
    return plain;
}

void session::socks5_request_processing(const lproxy::socks5::req& rq) {
    // dest_name 和 dest_port, dest_name 可能的值 为 ipv4/ipv6/域名
    switch (rq.AddrType) {
    case 0x01: {// ipv4
        // typedef array< unsigned char, 4 > bytes_type;
        ip::address_v4::bytes_type name_arr;
        ::memmove(name_arr.data(), 
                boost::get<socks5::Ipv4_t>(rq.DestAddr).ip, 4);
        ip::address_v4 ipv4(name_arr);
        this->dest_name = ipv4.to_string();
        break;
    }
    case 0x03: { // domain
        const data_t& domain_name = 
            boost::get<socks5::Domain_t>(rq.DestAddr).Name;
        this->dest_name.assign(domain_name.begin(), domain_name.end());
        /*
        this->dest_name.assign(rq.DestAddr.Domain.name.begin(),
              rq.DestAddr.Domain.name.end());
        */
        break;
    }
    case 0x04: {// ipv6
        // typedef array< unsigned char, 16 > bytes_type;
        ip::address_v6::bytes_type name_arr;
        ::memmove(name_arr.data(),
                boost::get<socks5::Ipv6_t>(rq.DestAddr).ip, 16);
        ip::address_v6 ipv6(name_arr);
        this->dest_name = ipv6.to_string();
        break;
    }
    default: throw socks5::illegal_data_type();
    }

    this->dest_port = rq.DestPort;

    switch (rq.Cmd) {
    case 0x01: // CONNECT请求
        this->socks5_cmd = CMD_CONNECT;
        resovle_connect_tcp(&this->dest_name[0], this->dest_port);
        // 异步成功才可，打包 socks5::resp 发给 local
        break;
    case 0x02: // BIND请求
        this->socks5_cmd = CMD_BIND;
        // 暂时不做
        // TODO
        // ...
        // 临时方案:
        this->socks5_cmd = CMD_UNSUPPORT;
        //throw lproxy::socks5::illegal_data_type();
        this->socks5_resp_reply = 0x07; // 不支持的命令

        break;
    case 0x03: {// UDP转发
        this->socks5_cmd = CMD_UDP;
        resovle_open_udp(&this->dest_name[0], this->dest_port);
        // 异步成功才可，打包 socks5::resp 发给 local

        break;
    }
    default:
        this->socks5_cmd = CMD_UNSUPPORT;
        this->socks5_resp_reply = 0x07; // 不支持的命令
    }

    // 打包 socks5::resp 发给 local
    if ((rq.Cmd != 0x01) && (rq.Cmd != 0x03)) { 
        // CMD_CONNECT 必须异步connect 执行结束后才能 打包
        // CMD_UDP     也必须异步connect 执行结束后才能 打包
        socks5_resp_to_local();
    }
}



// http://www.boost.org/doc/libs/1_36_0/doc/html/boost_asio/example/http/client/async_client.cpp
#include <boost/lexical_cast.hpp>
void session::resovle_connect_tcp(const char* name, uint16_t port) {
    ip::tcp::resolver rlv(this->socket_right_tcp.get_io_service());
    ip::tcp::resolver::query qry(name, boost::lexical_cast<std::string>(port));

    // async_resolve
    rlv.async_resolve(qry, boost::bind(&session::tcp_resolve_handler, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator));
}
void session::resovle_open_udp(const char* name, uint16_t port) {
    ip::udp::resolver rlv(this->socket_right_udp.get_io_service());
    ip::udp::resolver::query qry(name, boost::lexical_cast<std::string>(port));

    // async_resolve
    rlv.async_resolve(qry, boost::bind(&session::udp_resolve_handler, this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::iterator));
}

// resolve_handler
void session::tcp_resolve_handler(const boost::system::error_code& err, 
        tcp::resolver::iterator endpoint_iterator) {
    if (! err) {
        // Attempt a connection to the first endpoint in the list. Each endpoint
        // will be tried until we successfully establish a connection.
        tcp::endpoint endpoint = *endpoint_iterator;
        this->socket_right_tcp.async_connect(endpoint,
                boost::bind(&session::tcp_connect_handler, this,
                    boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else {
        std::cout << "Error: " << err.message() << "\n";
        this->socks5_resp_reply = 0x04; // 主机不可达
        socks5_resp_to_local();
    }
}

void session::udp_resolve_handler(const boost::system::error_code& err, 
        udp::resolver::iterator endpoint_iterator) {
    if (! err) {
        // Attempt a connection to the first endpoint in the list. Each endpoint
        // will be tried until we successfully establish a connection.
        udp::endpoint endpoint = *endpoint_iterator;
        this->socket_right_udp.async_connect(endpoint,
                boost::bind(&session::udp_connect_handler, this,
                    boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else {
        std::cout << "Error: " << err.message() << "\n";
        this->socks5_resp_reply = 0x04; // 主机不可达
        socks5_resp_to_local();
    }
}

// connect_handler
void session::tcp_connect_handler(const boost::system::error_code& err,
        tcp::resolver::iterator endpoint_iterator) {
    if (! err) {
        // The connection was successful. Send the request to local.
        // 连接成功
        this->socks5_resp_reply = 0x00;
        // 反馈给 local, 并将状态设置为连接
        socks5_resp_to_local();
    }
    else if (endpoint_iterator != tcp::resolver::iterator()) {
        // The connection failed. Try the next endpoint in the list.
        this->socket_right_tcp.close();
        tcp::endpoint endpoint = *endpoint_iterator;
        this->socket_right_tcp.async_connect(endpoint,
                boost::bind(&session::tcp_connect_handler, this,
                    boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else {
        std::cout << "Error: " << err.message() << "\n";
        this->socks5_resp_reply = 0x03; // 网络不可达
        socks5_resp_to_local();
    }
}
// udp 没有connect ??
// udp 可以有 connect 的通信方式, 可重复调用（tcp则不行），没有3次握手；
// udp connect 建立 一个端对端的连接，此后就可以和TCP一样使用send()/recv()
// 传递数据，而不需要每次都指定目标IP和端口号（sendto()/recvfrom()）, 以提升
// 效率；当然此后调用sendto()/recvfrom() 也是可以的。
void session::udp_connect_handler(const boost::system::error_code& err,
      udp::resolver::iterator endpoint_iterator) {
    if (! err) {
        // The connection was successful. Send the request to local.
        // 连接成功
        udp::endpoint endpoint = *endpoint_iterator;
        this->dest_name = endpoint.address().to_string();

        // 
        auto&& upd_open_ip_protocol = ip::udp::v4();
        // TODO
        // if (config::get_instance().udp_open_ip_protocal() == "ipv6") {
        //  upd_open_ip_protocol = ip::udp::v6();
        // }
        //
        boost::system::error_code ec;
        this->socket_right_udp.open(upd_open_ip_protocol, ec);
        if (ec) { // An error occurred.
            this->socks5_resp_reply = 0x01; // 普通SOCKS服务器连接失败
        }
        else {
            this->socks5_resp_reply = 0x00; //  成功
        }


        // 反馈给 local, 并将状态设置为连接
        socks5_resp_to_local();
    }
    else if (endpoint_iterator != udp::resolver::iterator()) {
        // The connection failed. Try the next endpoint in the list.
        this->socket_right_udp.close();
        udp::endpoint endpoint = *endpoint_iterator;
        socket_right_udp.async_connect(endpoint,
                boost::bind(&session::udp_connect_handler, this,
                    boost::asio::placeholders::error, ++endpoint_iterator));
    }
    else {
        std::cout << "Error: " << err.message() << "\n";
        this->socks5_resp_reply = 0x03; // 网络不可达
        socks5_resp_to_local();
    }
}

lproxy::data_t& session::pack_socks5_resp(lproxy::data_t& data) {
    socks5::resp resp;
    // 应答字段
    resp.Reply = this->socks5_resp_reply;
    // TODO
    const sdata_t& type = config::get_instance().get_bind_addr_type();
    if (type == "ipv4") { resp.AddrType = 0x01; }
    else if (type == "ipv6") { resp.AddrType = 0x04; }
    else if (type == "domain") { resp.AddrType = 0x03; }
    else { resp.AddrType = 0x01; }
    
    const sdata_t& bind_addr = config::get_instance().get_bind_addr();
    uint16_t bindport  = config::get_instance().get_bind_port();

    ip::address addr;
    addr = ip::address::from_string(&bind_addr[0]);
    // 如果是 ipv4 if (type == "ipv4") TODO
        ip::address_v4 ipv4 = addr.to_v4();
        ip::address_v4::bytes_type&& ipv4_bytes = ipv4.to_bytes();
    // BindAddr
    resp.set_IPv4(ipv4_bytes.data(), 4);
    // BindPort
    resp.set_BindPort(bindport);

    /*
    data_t data_;
    resp.pack(data_);
    data.assign(data_.begin(), data_.end());
    */

    resp.pack(data);
    return data;
}

// 打包 socks5::resp 发给 local
void session::socks5_resp_to_local() {
    lproxy::data_t data;
    // socks5::resp 封包
    pack_socks5_resp(data);
    // lproxy::server::reply 封包
    auto& rply_data = pack_data(data, data.size());
    // 异步发给 local

    boost::asio::async_write(socket_left, rply_data.buffers(),
                boost::bind(&session::left_read_handler, this,
                    boost::asio::placeholders::error,
                    boost::asio::placeholders::bytes_transferred));
    if (this->socks5_resp_reply == 0x00) {
        // 设置当前 socks5 状态为: CONNECTED 
        this->socks5_state = lproxy::socks5::server::CONNECTED; 
    }
    else {
        // socks5 服务器  不能响应 客户端请求命令
        // TODO
        delete_this();
        // or this->socks5_state = lprxoy::socks5::server::OPENING; ????
    }
}
