/*************************************************************************
	> File Name:    mysecret.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/1/11 14:04:04
 ************************************************************************/

#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <algorithm>
#include <iterator>
#include <boost/algorithm/string.hpp>
#include "crypto/aes_crypto.h"
#include "crypto/base64_crypto.h"
//#include "crypto/rsa_crypto.h"
using namespace std;
using namespace crypto;

typedef std::vector<uint8_t> stream;

enum LogLevel {DEBUG = 0, INFO, WARNING, ERROR, FATAL};
std::string gen_log(LogLevel loglevel, const std::string& message) {
    const std::string Level[5] = {
        "[DEBUG]", "[INFO]", "[WARNING]", "[ERROR]", "[FATAL]"
    };
    return Level[loglevel] + ' ' + message;
}
std::string gen_log(LogLevel loglevel, const std::ostringstream& message) {
    return gen_log(loglevel, message.str());
}

/*
template<typename T>
void assert_pretty(T&& condition, const string& message = "") {
    if (! std::forward<T>(condition)) {
        if (message != "") {
            std::cerr << message << std::endl;
        }
        assert(std::forward<T>(condition));
    } 
}
*/

template<typename T>
T&& pretty(T&& condition, const string& message = "") {
    if (! std::forward<T>(condition)) {
        if (message != "") {
            std::cerr << message << std::endl;
        }
    } 
    return std::forward<T>(condition);
}

void usage(void) {
   //mysecret [-e file]
   //mysecret [-d file]
   //mysecret [-h, --help]
   cout << "this is a help message.\n";
}

std::string& trim(std::string& some) {
    //" \t\n\v\f\r";
    boost::algorithm::trim(some);
    return some;
}


int main(int argc, char* argv[]) {

    if (argc != 3) {
        usage();
        return 1;
    }

    enum { ENCODE, DECODE } mode;

    const std::string mode_arg = argv[1]; 
    if (mode_arg == "-e") {
        mode = ENCODE;
    }
    else if (mode_arg == "-d") {
        mode = DECODE;
    }
    else {
        usage();
        return 2;
    }

    const std::string file_in = argv[2]; 
    const std::string file_out = file_in + ".out";

    try {
        char key_buffer[1024] = {0};
        auto&& keyfile = std::ifstream("./key");
        assert(pretty(keyfile, gen_log(FATAL, "cannot open file: 'key'")));
        keyfile.getline(key_buffer, 
                sizeof(key_buffer) / sizeof(key_buffer[0]));
        std::string key = key_buffer;
        trim(key);
        assert(pretty(key.size(), gen_log(FATAL, "key is empty!")));
        std::cout << gen_log(DEBUG, "key : [" + key + ']') << std::endl;

        std::string input_buffer;
        std::ifstream is (file_in, std::ifstream::binary);
        assert(pretty(is, gen_log(FATAL,
                        "cannot open file: '" + file_in + "'")));
        {
            is.seekg (0, is.end);
            int length = is.tellg();
            is.seekg (0, is.beg);

            uint8_t buffer[length];
            memset(buffer, 0, length);
            is.read ((char*)buffer, length);
            if (is) {
                std::cout << gen_log(INFO, "all characters read successfully.")
                    << std::endl;
            }
            else {
                std::ostringstream oss;
                oss << "error: only " << is.gcount() << " could be read";
                assert(pretty(is, gen_log(FATAL, oss)));
            }
            is.close();

            input_buffer.assign(buffer, buffer + length);
            trim(input_buffer);
        } 

        Encryptor aes_encryptor(new Aes(std::string(key)));
        auto&& base64cryptor = Base64();
        stream output, input(input_buffer.begin(), input_buffer.end());
        switch (mode) {
        case ENCODE:
            aes_encryptor.encrypt(output, &input[0], input.size());
            base64cryptor.encrypt(output, &output[0], output.size());
            break;
        case DECODE:
            base64cryptor.decrypt(input, &input[0], input.size());
            aes_encryptor.decrypt(output, &input[0], input.size());
            break;
        default:
            usage();
            return -1;
        }
        std::copy(output.begin(), output.end(), 
                std::ostream_iterator<char>(std::cout));
        std::cout << std::endl;

        //std::string output_buffer(output.begin(), output.end());
        ////std::ofstream("./file.out", std::ofstream::out) << output_buffer;
        //std::ofstream(file_out, std::ios_base::out | std::ios_base::binary)
        //    << output_buffer;
        auto&& outfile = std::ofstream(file_out, 
                std::ios_base::out | std::ios_base::binary);
        assert(pretty(outfile, gen_log(FATAL,
                        "cannot open file: '" + file_out + "'")));

        
        outfile.write((const char*)&output[0], output.size());
        outfile.close();
        std::cout << gen_log(INFO, file_out) << std::endl;
    }
    catch (...) {
        throw;
    }
    return 0;
}
