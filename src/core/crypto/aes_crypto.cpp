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
#include <cryptopp/cryptlib.h> 
// for CryptoPP::AutoSeededRandomPool
// for CryptoPP::RandomNumberGenerator


//#include <cryptopp/hex.h>
//#ifndef CRYPTOPP_ENABLE_NAMESPACE_WEAK
//#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
//#endif
//#include <cryptopp/md5.h>
using namespace crypto;



// http://cryptopp.com/wiki/Advanced_Encryption_Standard
// https://cryptopp.com/wiki/CFB_Mode
Aes::Aes(const uint8_t* _key, size_t _key_len) : md5_key(0x00, md5_key_len) {
    assert(_key);
    std::string key = (const char*)_key;
    assert(key.length() == _key_len);

    md5_key.Assign(&(make_md5key(key)[0]), md5_key_len);

    initialize_cryptor();
}


Aes::Aes(const std::string& _key) 
    : md5_key(&(make_md5key(_key)[0]), md5_key_len) {

    initialize_cryptor();
}

void Aes::initialize_cryptor(void) {
    
    //CryptoPP::AutoSeededRandomPool rnd;
    CryptoPP::RandomNumberGenerator rnd;
    // Generate a random IV
    byte iv[CryptoPP::AES::BLOCKSIZE];
    rnd.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);

    aesEncryptor.SetKeyWithIV(md5_key, md5_key.size(), iv);
    aesDecryptor.SetKeyWithIV(md5_key, md5_key.size(), iv);
}

std::vector<uint8_t> Aes::make_md5key(const std::string& _key) {
    std::vector<uint8_t> md5_cipher;
    auto&& md5_encryptor = Md5();
    md5_encryptor.encrypt(md5_cipher, 
            (const uint8_t*)_key.c_str(), _key.size());
    assert(md5_cipher.size() == md5_key_len);
    return md5_cipher;
}



std::vector<uint8_t>& Aes::encrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, size_t src_len) {
    //this->execute(this->aesEncryptor, dest, src, src_len);
    assert(src);
    uint8_t output[src_len];
    memset(output, 0 , src_len);
    aesEncryptor.ProcessData(output, src, src_len);
    dest.assign(output, output + src_len);
    return dest;
}
std::vector<uint8_t>& Aes::decrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, size_t src_len) {
    //this->execute(this->aesDecryptor, dest, src, src_len);
    assert(src);
    uint8_t output[src_len];
    memset(output, 0 , src_len);
    aesDecryptor.ProcessData(output, src, src_len);
    dest.assign(output, output + src_len);
    return dest;
}

