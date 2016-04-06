#ifndef _RSA_CRYPTO_H_1
#define _RSA_CRYPTO_H_1
/*************************************************************************
	> File Name:    rsa_crypto.h
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/7 6:19:45
 ************************************************************************/

// RSA/OAEP-MGF1(SHA-1)

#include <string>
#include "crypto/encryptor.h"


#include "cryptopp/rsa.h"
//for CryptoPP::RSA;
//for CryptoPP::InvertibleRSAFunction;
//for CryptoPP::RSAES_OAEP_SHA_Encryptor;
//for CryptoPP::RSAES_OAEP_SHA_Decryptor;

#include "cryptopp/osrng.h"
//for CryptoPP::AutoSeededRandomPool;

#include "cryptopp/hex.h"
//for CryptoPP::HexEncoder

namespace crypto {

//
// RSA/OAEP-MGF1(SHA-1)
//
class Rsa;
class RsaKey {
public:
    // 模长, 推荐 1024,2048,4096
    enum size {
        bit512   =   512,
        bit1024  =  1024,
        bit2048  =  2048,
        bit4096  =  4096,
        bit8192  =  8192,
        bit11776 = 11776,
        bit15360 = 15360,
        bit30720 = 30720,
        bit61140 = 61140
    };

    // publicKey 和 privateKey 模长相同
    struct PairHex {
        std::string  publicKey;
        std::string  privateKey;
        RsaKey::size keysize;
    };
    struct Pair {
        CryptoPP::RSA::PublicKey  publicKey;
        CryptoPP::RSA::PrivateKey privateKey;
        RsaKey::size              keysize;
    };
public:
    explicit RsaKey(RsaKey::size keysize);

    inline RsaKey::Pair          keyPair(void);
    inline const RsaKey::Pair    keyPair(void) const;

    inline RsaKey::PairHex       keyPairHex(void);
    inline const RsaKey::PairHex keyPairHex(void) const;

    inline std::string           publicKeyHex(void);
    inline const std::string     publicKeyHex(void) const;

    inline std::string           privateKeyHex(void);
    inline const std::string     privateKeyHex(void) const;

    inline CryptoPP::RSA::PublicKey       publicKey(void);
    inline const CryptoPP::RSA::PublicKey publicKey(void) const;

    inline CryptoPP::RSA::PrivateKey       privateKey(void);
    inline const CryptoPP::RSA::PrivateKey privateKey(void) const;

    inline RsaKey::size       keySize(void);
    inline const RsaKey::size keySize(void) const;

private:
    friend class Rsa;
    CryptoPP::AutoSeededRandomPool  rng;
    CryptoPP::InvertibleRSAFunction parameters;
    RsaKey::size                    keysize;
}; // class RsaKey

class Rsa : public Encrypt {
public:
    Rsa(const Rsa&) = delete;
    Rsa& operator= (const Rsa&) = delete;
    
    explicit Rsa(const RsaKey& rsakey);
    explicit Rsa(const RsaKey::Pair& keyPair);
    explicit Rsa(RsaKey::Pair&& keyPair);

    Rsa(RsaKey::size keysize, const std::string& publicKey);

    Rsa(RsaKey::size keysize,
            const uint8_t* publicKey, std::size_t publicKey_len);

    Rsa(RsaKey::size keysize,
            const std::string& publicKey, const std::string& privateKey);

    Rsa(RsaKey::size keysize,
            const uint8_t* publicKey, std::size_t publicKey_len, 
            const uint8_t* privateKey, std::size_t privateKey_len);

    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
private:
    CryptoPP::RSA::PublicKey       publicKey_;
    CryptoPP::RSA::PrivateKey      privateKey_;
    RsaKey::size                   keysize_;
    bool                           flag_privateKey_set_;
    CryptoPP::AutoSeededRandomPool rng_;
}; // class Rsa


} // namespace crypto

namespace crypto {
// inline function
RsaKey::Pair RsaKey::keyPair(void) {
    CryptoPP::RSA::PublicKey  publicKey( parameters );
    CryptoPP::RSA::PrivateKey privateKey( parameters );
    return { publicKey, privateKey, keysize };
}

const RsaKey::Pair RsaKey::keyPair(void) const {
    return const_cast<RsaKey*>(this)->keyPair();
}

RsaKey::PairHex RsaKey::keyPairHex(void) {
    return { publicKeyHex(), privateKeyHex(), keysize };
}
const RsaKey::PairHex RsaKey::keyPairHex(void) const {
    return const_cast<RsaKey*>(this)->keyPairHex();
}

std::string RsaKey::publicKeyHex(void) {
    std::string keyHex;
    CryptoPP::RSA::PublicKey  publicKey( parameters );
    publicKey.Save(CryptoPP::HexEncoder(
                new CryptoPP::StringSink(keyHex)).Ref()); 
    return keyHex;
}

const std::string RsaKey::publicKeyHex(void) const {
    return const_cast<RsaKey*>(this)->publicKeyHex();
}

std::string RsaKey::privateKeyHex(void) {
    std::string keyHex;
    CryptoPP::RSA::PrivateKey  privateKey( parameters );
    privateKey.Save(CryptoPP::HexEncoder(
                new CryptoPP::StringSink(keyHex)).Ref()); 
    return keyHex;
}

const std::string RsaKey::privateKeyHex(void) const {
    return const_cast<RsaKey*>(this)->privateKeyHex();
}


CryptoPP::RSA::PublicKey RsaKey::publicKey(void) {
    return CryptoPP::RSA::PublicKey(parameters);
}   
const CryptoPP::RSA::PublicKey RsaKey::publicKey(void) const {
    return const_cast<RsaKey*>(this)->publicKey();
}   
CryptoPP::RSA::PrivateKey RsaKey::privateKey(void) {
    return CryptoPP::RSA::PrivateKey(parameters);
}
const CryptoPP::RSA::PrivateKey RsaKey::privateKey(void) const {
    return const_cast<RsaKey*>(this)->privateKey();
}
RsaKey::size RsaKey::keySize(void) {
    return this->keysize;
}
const RsaKey::size RsaKey::keySize(void) const {
    return const_cast<RsaKey*>(this)->keySize(); 
}

} // namespace crypto

#endif // _RSA_CRYPTO_H_1
