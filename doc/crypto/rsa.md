crypto/rsa_crypto.h

----------

# crypto::RsaKey

```cpp
class crypto::RsaKey;
```
`crypto::Rsa` 所需的 key. 该类依赖 `cryptopp`

### crypto::RsaKey 类摘要

```cpp
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
	// 构造函数
    explicit RsaKey(RsaKey::size keysize);

	// get 方法
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
    inline const RsaKey::size keySize(void) const

private:
    friend class Rsa;
    CryptoPP::AutoSeededRandomPool  rng;
    CryptoPP::InvertibleRSAFunction parameters;
    RsaKey::size                    keysize;
}; // class crypto::RsaKey	
```

key 支持的模长：

| 模长                   | bit 位数|
|------------------------|---------|
|crypto::RsaKey::bit512  |   512   |
|crypto::RsaKey::bit1024 |   1024  | 
|crypto::RsaKey::bit2048 |   2048  |
|crypto::RsaKey::bit4096 |   4096  |
|crypto::RsaKey::bit8192 |   8192  |
|crypto::RsaKey::bit11776|   11776 |
|crypto::RsaKey::bit15360|   15360 |
|crypto::RsaKey::bit30720|   30720 |
|crypto::RsaKey::bit61140|   61140 |

模长位数越大越安全，构造所花费的时间也就越。

**注意，这里的`模长`指的是`公钥`的模长，下同。**



# crypto::Rsa

```cpp
class crypto::Rsa : public crypto::Encrypt;
```
采用 RSA/OAEP-MGF1(SHA-1) 非对称加解密算法的加解密类. 该类依赖 `cryptopp`

### crypto::Rsa 类摘要

```cpp
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
    CryptoPP::RSA::PublicKey       publicKey_;  // 公钥
    CryptoPP::RSA::PrivateKey      privateKey_; // 私钥
    RsaKey::size                   keysize_;    // 公钥模长
    bool                           flag_privateKey_set_; // 是否配置了私钥
    CryptoPP::AutoSeededRandomPool rng_;
}; // class crypto::Rsa
```

构造函数说明

* `explicit Rsa::Rsa(const RsaKey& rsakey);`

	`rsakey`: 用 RsaKey 对象构造 Rsa

* `explicit Rsa::Rsa(const RsaKey::Pair& keyPair);` 和 `explicit Rsa::Rsa(RsaKey::Pair&& keyPair);`

	`keyPair`: 用 RsaKey::Pair 对象构造 Rsa

* `Rsa::Rsa(RsaKey::size keysize, const std::string& publicKey);`

	1. `keysize`: key 的模长
	2. `publicKey`: 公钥

	用公钥构造 Rsa，如果构造失败会抛出加解密异常；使用该构造方法构造的 Rsa 对象，因为缺少私钥，所以不具有 解密 的能力， 如果尝试解密操作，会引发“解密时异常”`DecryptException`，用法参见 `demo`
	
* 用模长、公钥和秘钥 构造 Rsa 对象：

	* `Rsa::Rsa(RsaKey::size keysize, const std::string& publicKey, const std::string& privateKey);` 

	* `Rsa::Rsa(RsaKey::size keysize, const uint8_t* publicKey, std::size_t publicKey_len, const uint8_t* privateKey, std::size_t privateKey_len);`

	