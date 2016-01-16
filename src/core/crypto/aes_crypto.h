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
#include <cryptopp/modes.h>    // for CryptoPP::CFB_Mode<AES>

namespace crypto {

class Aes : public Encrypt {

static_assert(CryptoPP::AES::BLOCKSIZE > 0, 
        "illegal value: CryptoPP::AES::BLOCKSIZE");    

public:
    Aes(const uint8_t* _key, size_t _key_len);
    explicit Aes(const std::string& _key);
    Aes(const Aes&) = delete;
    Aes& operator= (const Aes&) = delete;

    /*
    virtual uint8_t* encrypt(uint8_t* dest, 
                        const uint8_t* src, size_t src_len);
    virtual uint8_t* decrypt(uint8_t* dest, 
                        const uint8_t* src, size_t src_len);
    */
    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
                        const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
                        const uint8_t* src, size_t src_len);
private:
    void initialize_cryptor(void);
    std::vector<uint8_t> make_md5key(const std::string& key);
private:
    
    CryptoPP::SecByteBlock  md5_key;
    static const size_t     md5_key_len = 32;

    // CFB mode
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption aesEncryptor;
    CryptoPP::CFB_Mode<CryptoPP::AES>::Decryption aesDecryptor;
 
};

} // namespace crypto

#endif
