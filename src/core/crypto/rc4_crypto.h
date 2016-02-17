#ifndef _RC4_CRYPTO_H_1
#define _RC4_CRYPTO_H_1
/*************************************************************************
	> File Name:    rc4_crypto.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/6 7:44:41
 ************************************************************************/
#include "crypto/encryptor.h"
#include "crypto/rc4.h"
namespace crypto { 

// 暂时不能交叉加解密
class Rc4 : public Encrypt {
public:
    Rc4(const uint8_t* _key, size_t _key_len);
    Rc4(const Rc4&) = delete;
    Rc4& operator= (const Rc4&) = delete;
    
    //virtual uint8_t* encrypt(uint8_t* dest, const uint8_t* src, size_t src_len);
    //virtual uint8_t* decrypt(uint8_t* dest, const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
private:
    const uint8_t*  key;
    size_t          key_len;
    // 内置同步机制，无法交叉加解密
    rc4_state       en_state, de_state;
};

} // namespace crypto 
#endif // _RC4_CRYPTO_H_1
