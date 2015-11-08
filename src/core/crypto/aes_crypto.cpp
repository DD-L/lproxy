/*************************************************************************
	> File Name:    aes_crypto.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/7 16:16:22
 ************************************************************************/
#include "crypto/aes_crypto.h"
#include <assert.h>
#include <type_traits>
#include <cryptopp/hex.h>
#ifndef CRYPTOPP_ENABLE_NAMESPACE_WEAK
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#endif
#include <cryptopp/md5.h>
using namespace crypto;
Aes::Aes(const uint8_t* _key, size_t _key_len) {
    assert(_key);
    std::string key = (const char*)_key;
    assert(key.length() == _key_len);
    make_md5key(key);
    aesEncryptor.SetKey(md5_key, md5_key_len);
    aesDecryptor.SetKey(md5_key, md5_key_len);
}

Aes::Aes(const std::string& _key) {
    make_md5key(_key);
    aesEncryptor.SetKey(md5_key, md5_key_len);
    aesDecryptor.SetKey(md5_key, md5_key_len);
}

void Aes::make_md5key(const std::string& _key) {
    CryptoPP::Weak::MD5 hash;
    byte digest[ CryptoPP::Weak::MD5::DIGESTSIZE ]; // 16
    hash.CalculateDigest( digest, (byte*) _key.c_str(), _key.length() );

    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach( new CryptoPP::StringSink( output ) );
    encoder.Put( digest, sizeof(digest) );
    encoder.MessageEnd();

    md5_key = (const uint8_t*)output.c_str();
    assert(output.length() == md5_key_len);
}


uint8_t* Aes::encrypt(uint8_t* dest, const uint8_t* src, size_t src_len) {
    this->execute(this->aesEncryptor, dest, src, src_len);
    return dest;
}
uint8_t* Aes::decrypt(uint8_t* dest, const uint8_t* src, size_t src_len) {
    this->execute(this->aesDecryptor, dest, src, src_len);
    return dest;
}

template<typename _aes_cryptor>
void Aes::execute(_aes_cryptor& cryptor, 
        uint8_t* dest, const uint8_t* src, size_t src_len) {

    static_assert(std::is_same<_aes_cryptor, CryptoPP::AESEncryption>::value 
            or std::is_same<_aes_cryptor, CryptoPP::AESDecryption>::value, 
            "the type '_aes_cryptor' must be one of 'CryptoPP::AESEncryption',"
            "'CryptoPP::AESDecryption'.");

    byte xorBlock[CryptoPP::AES::BLOCKSIZE] = {0};
    byte inBlock[CryptoPP::AES::BLOCKSIZE] = {0};
    byte outBlock[CryptoPP::AES::BLOCKSIZE] = {0};

    memset(xorBlock, 0, CryptoPP::AES::BLOCKSIZE);

    const size_t block_size = CryptoPP::AES::BLOCKSIZE;
    size_t       offset     = CryptoPP::AES::BLOCKSIZE;

    const size_t max_loop = ((src_len % block_size) == 0) 
        ? src_len / block_size 
        : size_t((float)src_len / (float)block_size + 1.0f);

    for (size_t loop = 0; loop < max_loop; ++loop) {

        const size_t unproc_data = src_len - loop * block_size;
        offset = ((unproc_data % block_size) == 0) ? block_size : unproc_data;

        memset(inBlock, 0, block_size);
        memmove((char*)inBlock, (const char*)(src + loop * offset), offset);

        cryptor.ProcessAndXorBlock(inBlock, xorBlock, outBlock);

        memmove((char*)(dest + loop * offset), (char*)outBlock, offset);
    }
}



