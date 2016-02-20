#ifndef _BASE64_CRYPTO_H_1
#define _BASE64_CRYPTO_H_1
/*************************************************************************
	> File Name:    base64_crypto.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/1/15 8:26:49
 ************************************************************************/

#include "crypto/encryptor.h"


namespace crypto {

class Base64 : public Encrypt {
public:
    Base64(const Base64&) = delete;
    Base64& operator= (const Base64&) = delete;
    Base64(void);

    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
}; // class Base64

} // namespace crypto

#endif // _BASE64_CRYPTO_H_1
