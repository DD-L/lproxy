/*************************************************************************
	> File Name:    xor_crypto.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/6 7:44:41
 ************************************************************************/
#include "crypto/xor_crypto.h"

namespace crypto { 

Xor::Xor(const uint8_t* _key, size_t _key_len, bool _is_synchro/*=false*/)
        : key(_key), key_len(_key_len), 
          encry_loc(0), decry_loc(0), is_synchro(_is_synchro) {}
/*
uint8_t* Xor::encrypt(uint8_t* dest, const uint8_t* src, size_t src_len) {
    size_t* loc = &encry_loc;
    return xor_encrypt(dest, src, src_len, loc);
}
uint8_t* Xor::decrypt(uint8_t* dest, const uint8_t* src, size_t src_len) {
    size_t* loc = &decry_loc;
    return xor_encrypt(dest, src, src_len, loc);
}
uint8_t* Xor::xor_encrypt(uint8_t* dest, const uint8_t* src, 
        size_t src_len, size_t* loc) {

    for (size_t i = 0; i < src_len; ++i) {
        *(dest + i) = src[i] ^ key[(i + *loc) % key_len];
    }
    (is_synchro) && (*loc = (src_len + *loc) % key_len);
    return dest;
}
*/

std::vector<uint8_t>& Xor::encrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, size_t src_len) {
    size_t* loc = &encry_loc;
    return xor_encrypt(dest, src, src_len, loc);
}
std::vector<uint8_t>& Xor::decrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, size_t src_len) {
    size_t* loc = &decry_loc;
    return xor_encrypt(dest, src, src_len, loc);
}
std::vector<uint8_t>& Xor::xor_encrypt(std::vector<uint8_t>& dest, 
        const uint8_t* src, size_t src_len, size_t* loc) {
    dest.assign(src_len, 0);
    for (size_t i = 0; i < src_len; ++i) {
        dest[i] = src[i] ^ key[(i + *loc) % key_len];
    }
    (is_synchro) && (*loc = (src_len + *loc) % key_len);
    return dest;
}
} // namespace crypto 
