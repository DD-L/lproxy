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
    // LF 编码输出中是否允许包含换行符, 默认不允许包含换行符
    // true: 编码输出包含换行符; false: 编码输出包含换行符  
    Base64(bool allow_LF = false);

    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
private:
    // RFC2045 标准规定:
    // The encoded output stream must be represented in lines of no more
    // than 76 characters each.
    bool LF; // true: 编码输出包含换行符; false: 编码输出包含换行符  
}; // class Base64

} // namespace crypto

#endif // _BASE64_CRYPTO_H_1
