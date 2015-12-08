#ifndef _ENCRYPTOR_H_1
#define _ENCRYPTOR_H_1
/*************************************************************************
	> File Name:    encryptor.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/6 7:44:41
 ************************************************************************/
#include <stddef.h>
#include <stdint.h>
#include <memory>
#include <vector>
namespace crypto { 

/**
 * class Encrypt
 *  不同加密类型的接口类
 */ 
class Encrypt {
public:
    Encrypt(void) {} 
    Encrypt(const Encrypt&) = delete;
    Encrypt& operator= (const Encrypt&) = delete;
    virtual ~Encrypt() {}

    //virtual uint8_t* encrypt(uint8_t* dest, const uint8_t* src, size_t src_len) = 0;
    //virtual uint8_t* decrypt(uint8_t* dest, const uint8_t *src, size_t src_len) = 0;
    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len) = 0;
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len) = 0;
};

/**
 * class Encryptor
 *  加密/解密 器
 */ 
class Encryptor {
public:
    explicit Encryptor(Encrypt* _encrypt);
    virtual ~Encryptor() {}
    //uint8_t* encrypt(uint8_t* dest, const uint8_t* src, size_t src_len);
    //uint8_t* decrypt(uint8_t* dest, const uint8_t* src, size_t src_len);
    std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
private:
    std::shared_ptr<Encrypt> crypto;
};

} // namespace crypto 


#endif // _ENCRYPTOR_H_1
