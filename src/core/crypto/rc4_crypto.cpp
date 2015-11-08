/*************************************************************************
	> File Name:    rc4_crypto.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/6 7:44:41
 ************************************************************************/

#include <assert.h>
#include "crypto/rc4_crypto.h"
namespace crypto { 
Rc4::Rc4(const uint8_t* _key, size_t _key_len)
        : key(_key), key_len(_key_len) {
    assert(key);
    rc4_init(&en_state, key, key_len);
    rc4_init(&de_state, key, key_len);
}

uint8_t* Rc4::encrypt(uint8_t* dest, const uint8_t* src, size_t src_len) {
    rc4_crypt(&en_state, src, dest, src_len);
    return dest;
}
uint8_t* Rc4::decrypt(uint8_t* dest, const uint8_t* src, size_t src_len) {
    rc4_crypt(&de_state, src, dest, src_len);
    return dest;
}
} // namespace crypto 
