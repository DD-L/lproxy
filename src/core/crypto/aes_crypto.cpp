/*************************************************************************
	> File Name:    aes_crypto.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/7 16:16:22
 ************************************************************************/
#include "crypto/aes_crypto.h"
#include <assert.h>
#include <type_traits>

#include "crypto/md5_crypto.h" // for md5
//#include <cryptopp/cryptlib.h> 
//// for CryptoPP::RandomNumberGenerator
#include <cryptopp/osrng.h>
// for CryptoPP::AutoSeededRandomPool

using namespace crypto;

// http://cryptopp.com/wiki/Advanced_Encryption_Standard
// https://cryptopp.com/wiki/CFB_Mode
// https://www.cryptopp.com/wiki/CTR_Mode

Aes::Aes(const uint8_t* _key, std::size_t _key_len) 
        : aes_key(0x00, aes_key_len) {
    assert(_key);
    std::string key = (const char*)_key;
    assert(key.length() == _key_len);

    aes_key.Assign(&(make_md5cipher(key)[0]), aes_key_len);

    initialize_counter();
}

Aes::Aes(const std::string& _key) 
    : aes_key(&(make_md5cipher(_key)[0]), aes_key_len) {

    initialize_counter();
}

void Aes::initialize_counter(void) {
    CryptoPP::AutoSeededRandomPool rnd;
    rnd.GenerateBlock(ctr, CryptoPP::AES::BLOCKSIZE);
}

std::vector<uint8_t> Aes::make_md5cipher(const std::string& _key) {
    std::vector<uint8_t> md5_cipher;
    auto&& md5_encryptor = Md5();
    md5_encryptor.encrypt(md5_cipher, 
            (const uint8_t*)_key.c_str(), _key.size());
    assert(md5_cipher.size() == aes_key_len);
    return md5_cipher;
}

std::vector<uint8_t>& Aes::encrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, std::size_t src_len) {
    assert(src);
    uint8_t output[src_len];
    memset(output, 0 , src_len);

    CryptoPP::CTR_Mode<CryptoPP::AES>::Encryption aesEncryptor;
    aesEncryptor.SetKeyWithIV(aes_key, aes_key.size(), ctr, sizeof(ctr));
    aesEncryptor.ProcessData(output, src, src_len);
    dest.assign(output, output + src_len);
    
    /*
    std::string output;
    CryptoPP::StringSource ss1(src, src_len, true,
            new CryptoPP::StreamTransformationFilter(aesEncryptor,
                new CryptoPP::StringSink(output)
            ) // CryptoPP::StreamTransformationFilter
    ); // CryptoPP::StringSource
    dest.assign(output.begin(), output.end());
    assert(dest.size() == src_len);
    */
    return dest;
}

std::vector<uint8_t>& Aes::decrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, std::size_t src_len) {
    assert(src);
    uint8_t output[src_len];
    memset(output, 0 , src_len);

    CryptoPP::CTR_Mode<CryptoPP::AES>::Decryption aesDecryptor;
    aesDecryptor.SetKeyWithIV(aes_key, aes_key.size(), ctr, sizeof(ctr));
    aesDecryptor.ProcessData(output, src, src_len);
    dest.assign(output, output + src_len);

    /*
    std::string output;
    CryptoPP::StringSource ss1(src, src_len, true,
            new CryptoPP::StreamTransformationFilter(aesDecryptor,
                new CryptoPP::StringSink(output)
            ) // CryptoPP::StreamTransformationFilter
    ); // CryptoPP::StringSource
    dest.assign(output.begin(), output.end());
    assert(dest.size() == src_len);
    */
    return dest;
}

