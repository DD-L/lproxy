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
#include <cryptopp/modes.h>    
// for CryptoPP::CFB_Mode<Crypto::AES>
// for CryptoPP::CTR_Mode<Crypto::AES>

namespace crypto {

// CTR mode
class Aes : public Encrypt {

static_assert(CryptoPP::AES::BLOCKSIZE > 0, 
        "illegal value: CryptoPP::AES::BLOCKSIZE");    

public:
    Aes(const uint8_t* _key, std::size_t _key_len);
    explicit Aes(const std::string& _key);
    Aes(const Aes&) = delete;
    Aes& operator= (const Aes&) = delete;

    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
                        const uint8_t* src, std::size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
                        const uint8_t* src, std::size_t src_len);
private:
    void initialize_counter(const std::string& something);
    std::vector<uint8_t> make_md5cipher(const std::string& key);
private:
    
    CryptoPP::SecByteBlock       aes_key;
    static const std::size_t     aes_key_len = 32;

    // counter
    uint8_t ctr[CryptoPP::AES::BLOCKSIZE];
    // the size is always 16 bytes
 
}; // class crypto::Aes

} // namespace crypto

#endif
