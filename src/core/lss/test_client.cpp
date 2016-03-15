/*************************************************************************
	> File Name:    test_client.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/3/10 9:06:52
 ************************************************************************/
#include <lss/typedefine.h>
//#include <boost/asio.hpp>
#include <iostream>
#include <stdint.h>
#include <cstdio>
#include <thread>
#include <sstream> // for getline
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string.hpp>
using namespace std;
using namespace boost::asio;

typedef uint8_t                 byte;
typedef std::basic_string<byte> data_t;

class uint32_from_hex   // For use with boost::lexical_cast
{
    uint32_t value;
public:
    operator uint32_t() const { return value; }
    friend std::istream& operator>>(std::istream& in, 
            uint32_from_hex& outValue )
    {
        in >> std::hex >> outValue.value;
        return in;
    }
};


data_t& read_input(data_t& data) {
    string buff, flag;

    getline(cin, buff);
    flag.assign(buff.begin(), buff.begin() + 3);
    if (flag == "str") {
        data.assign(buff.begin() + 4, buff.end()); 
        lproxy::_debug_print_data(data, char(), 0);
    }
    else if (flag == "hex" || flag == "dec") {
        string buf(buff.begin() + 4, buff.end());
        stringstream ss(buf), sub;
        while (getline(ss, buf, ' ')) {
            try {
                boost::algorithm::trim(buf);
                uint32_t v;
                if (flag == "hex") {
                    v = boost::lexical_cast<uint32_from_hex>(buf);
                }
                else {
                    v = boost::lexical_cast<uint32_t>(buf);
                }
                if (v > 255 || v < 0) {
                    cout << "wrong data: " << v << endl;
                    data = data_t();
                    return read_input(data);
                }
                data.push_back((byte)v); 
            }
            catch (boost::bad_lexical_cast& e) {
                cout << e.what() << endl;
            }
        }
    }
    else {
        cout << "['hex ...'] ['dec ...'] ['str ...']" << endl; 
        data = data_t();
        return read_input(data);
    }
    lproxy::_debug_print_data(data, int(), ' ', std::hex);
    return data; 
}

string read_ip_addr(string& ip) {
    cout << "input connect ip: ";
    cin >> ip;
    getchar();
    return ip;
}
uint16_t read_port(uint16_t& port) {
    cout << "input connect port: ";
    cin >> port;
    getchar();
    return port;
}

int main() 
try {
    lproxy::io_service io_service;
    std::thread thread([&io_service](void) -> void {
               lproxy::io_service::work work(io_service); 
               io_service.run(); 
               cout << "thread exit" << endl;
            });
    lproxy::ip::tcp::socket sock(io_service);
    string ip; uint16_t port;
    read_ip_addr(ip); read_port(port);
    lproxy::ip::tcp::endpoint ep(ip::address::from_string(ip), port);
    boost::system::error_code ec;
    sock.connect(ep, ec);
    if (ec) {
        cout << ec.message() << endl;
        return 1;
    }
    cout << "conneted " << ip << ":" << port << endl;
    while (true) {
        cout << "send: ['hex ...'] ['dec ...'] ['str ...']" << endl;
        data_t data_send; 
        read_input(data_send);
        boost::asio::write(sock, 
                buffer(&data_send[0], data_send.size()), ec);
        if (ec) {
            cout << ec.message() << endl;
            return 2;
        }
        //
        data_t data_receive(1024, 0);
        std::size_t bytes_transferred 
            = sock.read_some(buffer(&data_receive[0], 1024), ec);
        if (ec) {
            cout << ec.message() << endl;
            return 3;
        }
        cout << "received: " << bytes_transferred << " byte:\n";
        lproxy::_debug_print_data(data_receive, char(), 0);
        lproxy::_debug_print_data(data_receive, int(), ' ', std::hex);
    }
    return 0;
}
catch (...) {
    cout << "FATAL" << endl;
}

