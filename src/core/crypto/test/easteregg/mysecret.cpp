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

const string __VER__ = "0.0.1";

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

std::string& trim(std::string& some) {
    //" \t\n\v\f\r";
    boost::algorithm::trim(some);
    return some;
}

void read_input_file(std::ifstream& is, std::string& input_buffer) {
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

// read line
void get_key(std::string& key) {
    // read the key from file -'./key'
    char key_buffer[1024] = {0};
    auto&& keyfile = std::ifstream("./key");
    assert(pretty(keyfile, gen_log(FATAL, "cannot open file: 'key'")));
    keyfile.getline(key_buffer, 
            sizeof(key_buffer) / sizeof(key_buffer[0]));
    key = key_buffer;
    trim(key);
    assert(pretty(key.size(), gen_log(FATAL, "key is empty!")));
    std::cout << gen_log(DEBUG, "key : [" + key + ']') << std::endl;
}

void usage(const std::string& appname) {
   cout << "\n'mysecret' will auto-load the './key' file from the current\n"
       "working path, then complete the encryption and decryption\n"
       "tasks with the key...\n\n";
   const std::string message =
       "Usage:\n"
       "  " + appname + " [option, ..] [ file | text ]\n"
       "  e.g.\n"
       "    " + appname + " -e -f plain_file_name\n"
       "    " + appname + " -d -f cipher_file_name\n"
       "    " + appname + " -e -s plain_text\n"
       "    " + appname + " -d -s cipher_text\n"
       "    " + appname + " -h\n"
       "    " + appname + " -v\n"
       "option:\n"
       "  -h, --help           Show this message.\n"
       "  -v, --version        Show version.\n"
       "  -e, --encrypt        Encode.\n"
       "  -d, --decrypt        Decode.\n"
       "  -f, --file           File.\n"
       "  -s, --string         String.\n";
   std::cout << message << std::endl;
}

int main(int argc, char* argv[]) {
    const std::string  appname = argv[0];
    const std::string& version = __VER__;

    if ((2 != argc) && (4 != argc)) {
        usage(appname);
        return 1;
    }

    enum { ENCODE, DECODE, HELP, VERSION } mode;
    enum { FROMFILE, FROMTEXT } from;

    // argv[1], mode
    const std::string mode_arg = argv[1]; 
    if ("-e" == mode_arg || "--encrypt" == mode_arg) {
        mode = ENCODE;
    }
    else if ("-d" == mode_arg || "--decrypt" == mode_arg) {
        mode = DECODE;
    }
    else if ("-h" == mode_arg || "--help" == mode_arg) {
        mode = HELP;
        usage(appname);
        return 0;
    }
    else if ("-v" == mode_arg || "--version" == mode_arg) {
        mode = VERSION;
        std::cout << appname << " v" + version << std::endl;
        return 0;
    }
    else {
        usage(appname);
        return 2;
    }

    // argv[2], from
    const std::string filextext = argv[2];
    if ("-f" == filextext || "--file" == filextext) {
        from = FROMFILE;
    }
    else if ("-s" == filextext || "--string" == filextext) {
        from = FROMTEXT;
    }
    else {
        usage(appname);
        return 2;
    }

    std::string input_buffer;      // 
    std::string file_in, file_out; // file name
    switch (from) {
    case FROMFILE:
        file_in = argv[3];
        file_out = file_in + ".out";
        break;
    case FROMTEXT:
        input_buffer = argv[3];
        break;
    default:
        usage(appname);
        return 2;
    }

    try {
        // read the key from file -'./key'
        std::string key;
        get_key(key);

        // get input_buffer
        if (FROMFILE == from) {
            // read inputfile
            std::ifstream is (file_in, std::ifstream::binary);
            assert(pretty(is, gen_log(FATAL,
                            "cannot open file: '" + file_in + "'")));

            read_input_file(is, input_buffer);
        }

        // crypto
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
            usage(appname);
            return -1;
        }

        // output 
        std::copy(output.begin(), output.end(), 
                std::ostream_iterator<char>(std::cout));
        std::cout << std::endl;

        if (FROMFILE == from) {
            // write stream to output file
            auto&& outfile = std::ofstream(file_out, 
                    std::ios_base::out | std::ios_base::binary);
            assert(pretty(outfile, gen_log(FATAL,
                            "cannot open file: '" + file_out + "'")));

            outfile.write((const char*)&output[0], output.size());
            outfile.close();
            std::cout << gen_log(INFO, file_out) << std::endl;
        }
    }
    catch (...) {
        throw;
    }
    return 0;
}
