#ifndef _XOR_CRYPTO_H_1
#define _XOR_CRYPTO_H_1
/*************************************************************************
	> File Name:    xor_crypto.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/6 7:44:41
 ************************************************************************/
#include "crypto/encryptor.h"

namespace crypto { 

// 默认不开同步机制，可以交叉加解密
class Xor : public Encrypt {
public:
    Xor(const uint8_t* _key, size_t _key_len, bool _is_synchro = false); 
    Xor(const Xor&) = delete;
    Xor& operator= (const Xor&) = delete;
    
    //virtual uint8_t* encrypt(uint8_t* dest, const uint8_t* src, size_t src_len);
    //virtual uint8_t* decrypt(uint8_t* dest,  const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest,  
            const uint8_t* src, size_t src_len);
private:
    std::vector<uint8_t>& xor_encrypt(std::vector<uint8_t>& dest, 
                const uint8_t* src, size_t src_len, size_t* loc);
private:
    const uint8_t*  key;
    size_t          key_len;
    size_t          encry_loc;  // 加密同步用
    size_t          decry_loc;  // 解密同步用
    bool            is_synchro; // 是否启用同步模式 
}; 

} // namespace crypto 
#endif // _XOR_CRYPTO_H_1 

