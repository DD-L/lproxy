/*************************************************************************
	> File Name:    rsa_crypto.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/8 5:15:50
 ************************************************************************/

#include "crypto/rsa_crypto.h"
#include "except/except.h"

#include "cryptopp/filters.h"
//for CryptoPP::StringSink;
//for CryptoPP::StringSource;
//for CryptoPP::PK_EncryptorFilter;
//for CryptoPP::PK_DecryptorFilter;


#include "cryptopp/sha.h"
//for CryptoPP::SHA1;

#include "cryptopp/files.h"
//for CryptoPP::FileSink;
//for CryptoPP::FileSource;

#include "cryptopp/SecBlock.h"
//for CryptoPP::SecByteBlock;

#include "cryptopp/cryptlib.h"
//for CryptoPP::Exception;
//for CryptoPP::DecodingResult;

namespace crypto {

// class RsaKey
RsaKey::RsaKey(RsaKey::size keysize) {
    parameters.GenerateRandomWithKeySize(rng, (unsigned int)(keysize));
    this->keysize = keysize;
}
// class Rsa

Rsa::Rsa(const RsaKey& rsakey) 
    /*
    : publicKey_(rsakey.privateKey()), privateKey_(rsakey.publicKey()),
      keysize_(rsakey.keySize()), flag_privateKey_set_(true) {}
      */
    : publicKey_(rsakey.parameters), privateKey_(rsakey.parameters),
      keysize_(rsakey.keysize), flag_privateKey_set_(true) {}

Rsa::Rsa(const RsaKey::Pair& keyPair) 
    : publicKey_(keyPair.publicKey), privateKey_(keyPair.privateKey),
      keysize_(keyPair.keysize), flag_privateKey_set_(true) {}

Rsa::Rsa(RsaKey::Pair&& keyPair)
    : publicKey_(keyPair.publicKey), privateKey_(keyPair.privateKey),
      keysize_(keyPair.keysize), flag_privateKey_set_(true) {}

Rsa::Rsa(RsaKey::size keysize, const std::string& publicKey) {
    publicKey_.Load(CryptoPP::StringSource(publicKey, true, 
                new CryptoPP::HexDecoder()).Ref());
    keysize_             = keysize;
    flag_privateKey_set_ = false;
}

Rsa::Rsa(RsaKey::size keysize,
        const uint8_t* publicKey, std::size_t publicKey_len) {
    publicKey_.Load(CryptoPP::StringSource(publicKey, publicKey_len, true, 
                new CryptoPP::HexDecoder()).Ref());
    keysize_             = keysize;
    flag_privateKey_set_ = false;
}

Rsa::Rsa(RsaKey::size keysize,
        const std::string& publicKey, const std::string& privateKey) {
    publicKey_.Load(CryptoPP::StringSource(publicKey,   true, 
                new CryptoPP::HexDecoder()).Ref());
    privateKey_.Load(CryptoPP::StringSource(privateKey, true, 
                new CryptoPP::HexDecoder()).Ref());
    keysize_             = keysize;
    flag_privateKey_set_ = true;
}


Rsa::Rsa(RsaKey::size keysize,
        const uint8_t* publicKey,  std::size_t publicKey_len, 
        const uint8_t* privateKey, std::size_t privateKey_len) {
    publicKey_.Load(CryptoPP::StringSource(publicKey,   publicKey_len, 
                true, new CryptoPP::HexDecoder()).Ref());
    privateKey_.Load(CryptoPP::StringSource(privateKey, privateKey_len, 
                true, new CryptoPP::HexDecoder()).Ref());
    keysize_             = keysize;
    flag_privateKey_set_ = false;
}


// 公钥的模长            生成的密文  一次可加密的最大长度      
// public keysize(bit)   cipher(bit) source/block max len(byte)
// 512                   512         22
// 1024                  1024        86
// 2048                  2048        214
// 4096                  4096        470
// 8192                  8192        982
// 11776                 11776       1430
// 15360                 15360       1878
// ... 

// source_max_len_byte = keysize_bit / 8 - 42;


std::vector<uint8_t>& Rsa::encrypt(std::vector<uint8_t>& dest, 
                                    const uint8_t* src, size_t src_len)
try {
    const std::size_t max_block_size = std::size_t(keysize_) / 8 - 42; 
    std::size_t       offset = max_block_size;
    std::size_t       current = 0;

    const std::size_t max_loop = ((src_len % max_block_size) == 0) 
        ? src_len / max_block_size 
        : std::size_t((float)src_len / (float)max_block_size + 1.0f);
    dest.clear();
    std::string cipher;

    CryptoPP::RSAES_OAEP_SHA_Encryptor e(publicKey_);
    for (std::size_t loop = 0; loop < max_loop; ++loop) {
        const std::size_t unproc_data = src_len - loop * max_block_size;
        offset = 
            (unproc_data >= max_block_size) ? max_block_size : unproc_data;

        current =  src_len - unproc_data;
        cipher.clear();
        CryptoPP::StringSource(src + current, offset, true,
            new CryptoPP::PK_EncryptorFilter(rng_, e,
                    new CryptoPP::StringSink(cipher)
            )
        );
        
        dest.insert(dest.end(), cipher.begin(), cipher.end());
    }
    return dest;
}
catch(CryptoPP::Exception& e) {
    throw EncryptException(std::string("[rsa]") + e.what());
}


std::vector<uint8_t>& Rsa::decrypt(std::vector<uint8_t>& dest, 
                                    const uint8_t* src, size_t src_len)
try {
    if (! flag_privateKey_set_) {
        // private key 未设置
        throw DecryptException("[rsa]Private key is not set !"); 
    }
    const std::size_t block_size = std::size_t(keysize_) / 8;
    if ((src_len % block_size) != 0) {
        // 不合法的资源
        throw DecryptException("[rsa]The length of ciphertext "
                "is illegal !"); 
    }
    std::size_t       current = 0;
    const std::size_t max_loop = src_len / block_size;

    CryptoPP::RSAES_OAEP_SHA_Decryptor d(privateKey_);
    std::string recovered;
    dest.clear();
    for (std::size_t loop = 0; loop < max_loop; ++loop) {
        current     = loop * block_size; 
        recovered.clear();
        CryptoPP::StringSource( src + current, block_size, true,
            new CryptoPP::PK_DecryptorFilter(rng_, d,
                new CryptoPP::StringSink(recovered)
            ) // PK_EncryptorFilter
         ); // StringSource
        dest.insert(dest.end(), recovered.begin(), recovered.end());
    }
    
    return dest;
}
catch (CryptoPP::Exception& e) {
    throw DecryptException(std::string("[rsa]") + e.what());
}

} // namespace crypto
