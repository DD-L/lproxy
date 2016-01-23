#ifndef _MD5_CRYPTO_H_1
#define _MD5_CRYPTO_H_1
/*************************************************************************
	> File Name:    md5_crypto.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2016/1/15 14:11:56
 ************************************************************************/

#include "crypto/encryptor.h"

namespace crypto {

// 32位字节长度的 md5 加密. (大写)
class Md5 : public Encrypt {
public:
    Md5(const Md5&) = delete;
    Md5& operator= (const Md5&) = delete;
    Md5(void);
    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
private:
    static const size_t md5_len = 32;
}; // class Md5

} // namespace crypto
#endif // _MD5_CRYPTO_H_1
