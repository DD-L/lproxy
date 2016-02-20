/*************************************************************************
	> File Name:    md5_crypto.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/1/15 14:11:14
 ************************************************************************/
#include "crypto/md5_crypto.h"
#include "except/except.h"
#include <cryptopp/hex.h>
#ifndef CRYPTOPP_ENABLE_NAMESPACE_WEAK
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#endif
#include <cryptopp/md5.h>
using namespace crypto;

Md5::Md5(void) {}

std::vector<uint8_t>& Md5::encrypt(std::vector<uint8_t>& dest, 
    const uint8_t* src, size_t src_len) {

    CryptoPP::Weak::MD5 hash;
    byte digest[CryptoPP::Weak::MD5::DIGESTSIZE]; // 16
    hash.CalculateDigest(digest, src, src_len);

    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach(new CryptoPP::StringSink(output));
    encoder.Put(digest, sizeof(digest));
    encoder.MessageEnd();

    dest.assign(output.begin(), output.end());
    assert(dest.size() == md5_len); // 32

    return dest;
}
std::vector<uint8_t>& Md5::decrypt(std::vector<uint8_t>& dest, 
    const uint8_t* src, size_t src_len) {
    // md5 没有解密接口
    throw DecryptException("[md5] 'MD5' does not have decryption interface!");
    (void)src, (void)src_len;
    return dest;
}
