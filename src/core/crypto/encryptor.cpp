/*************************************************************************
	> File Name:    encryptor.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/6 7:44:41
 ************************************************************************/
#include <assert.h>
#include "crypto/encryptor.h"
namespace crypto { 

Encryptor::Encryptor(Encrypt* _encrypt) 
    : crypto(_encrypt) {
    assert(crypto);
}
uint8_t* Encryptor::encrypt(uint8_t* dest, const uint8_t* src, size_t src_len) {
    return crypto->encrypt(dest, src, src_len);
}
uint8_t* Encryptor::decrypt(uint8_t* dest, const uint8_t* src, size_t src_len) {
    return crypto->decrypt(dest, src, src_len);
}

} // namespace crypto 


