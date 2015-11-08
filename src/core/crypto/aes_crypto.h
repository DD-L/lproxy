#ifndef _AES_CRYPTO_H_1
#define _AES_CRYPTO_H_1
/*************************************************************************
	> File Name:    aes_crypto.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/7 9:17:52
 ************************************************************************/
#include <string>
#include "crypto/encryptor.h"
#include <cryptopp/aes.h>
namespace crypto {

class Aes : public Encrypt {

static_assert(CryptoPP::AES::BLOCKSIZE > 0, 
        "illegal value: CryptoPP::AES::BLOCKSIZE");    

public:
    Aes(const uint8_t* _key, size_t _key_len);
    Aes(const std::string& _key);
    Aes(const Aes&) = delete;
    Aes& operator= (const Aes&) = delete;

    virtual uint8_t* encrypt(uint8_t* dest, const uint8_t* src, size_t src_len);
    virtual uint8_t* decrypt(uint8_t* dest, const uint8_t* src, size_t src_len);
private:
    void make_md5key(const std::string& key);
    template<typename _aes_cryptor>
    void execute(_aes_cryptor& cryptor, 
            uint8_t* dest, const uint8_t* src, size_t src_len);
private:
    const uint8_t*          md5_key;
    static const size_t     md5_key_len = 32;
    CryptoPP::AESEncryption aesEncryptor; 
    CryptoPP::AESDecryption aesDecryptor;
 
};

} // namespace crypto

#endif
