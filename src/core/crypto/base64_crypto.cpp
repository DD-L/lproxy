/*************************************************************************
	> File Name:    base64_crypto.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/1/15 8:35:22
 ************************************************************************/

#include <string>
#include "crypto/base64_crypto.h"

#include <cryptopp/base64.h>
//for CryptoPP::Base64Encoder
//for CryptoPP::Base64Decoder
#include <cryptopp/filters.h>
//for CryptoPP::StringSink;
//for CryptoPP::StringSource;
//for CryptoPP::PK_EncryptorFilter;
//for CryptoPP::PK_DecryptorFilter;

namespace crypto {

Base64::Base64(void) {}

std::vector<uint8_t>& Base64::encrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, size_t src_len) {

    std::string output;
    CryptoPP::StringSource ss(src, src_len, true,
        new CryptoPP::Base64Encoder(
            new CryptoPP::StringSink(output)
        ) // CryptoPP::Base64Encoder
    ); // CryptoPP::StringSource

    dest.assign(output.begin(), output.end());
    return dest;
}

std::vector<uint8_t>& Base64::decrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, size_t src_len) {
    
    std::string output;
    CryptoPP::StringSource ss(src, src_len, true,
        new CryptoPP::Base64Decoder(
            new CryptoPP::StringSink(output)
        ) // CryptoPP::Base64Decoder
    ); // CryptoPP::StringSource

    dest.assign(output.begin(), output.end());
    return dest;
}

}// namespace crypto

