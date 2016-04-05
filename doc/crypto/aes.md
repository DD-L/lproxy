crypto/aes_crypto.h

-----------------

# crypto::Aes

```cpp
class crypto::Aes : public crypto::Encrypt;
```

采用 AES 256 bit CTR mode 算法的加解密类, 该类依赖 `cryptopp`

### crypto::Aes 类摘要

```cpp
class Aes : public Encrypt {

static_assert(CryptoPP::AES::BLOCKSIZE > 0, 
        "illegal value: CryptoPP::AES::BLOCKSIZE");

public:
    class raw256keysetting {};

public:
    Aes(const uint8_t* _key, const std::size_t _key_len);
    explicit Aes(const std::string& _key);
    Aes(const std::string& _raw256key, Aes::raw256keysetting);
    Aes(const std::vector<uint8_t>& _raw256key, Aes::raw256keysetting);
    Aes(const Aes&) = delete;
    Aes& operator= (const Aes&) = delete;

    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
                        const uint8_t* src, std::size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
                        const uint8_t* src, std::size_t src_len);
private:
    void initialize_counter(const std::string& something);
    std::vector<uint8_t> make_md5cipher(const std::string& key);
private:
    
    CryptoPP::SecByteBlock       aes_key;
    static const std::size_t     aes_key_len = 32; // 256 bit

    // counter
    uint8_t ctr[CryptoPP::AES::BLOCKSIZE];
    // the size is always 16 bytes
 
}; // class crypto::Aes
```

支持并行加解密模式

构造函数说明

1. `Aes::Aes(const uint8_t* _key, std::size_t _key_len);`
	
	* `_key`： key 的首地址
	* `_key_len`：key 的长度

	对 key 的长度没有限制

2. `explicit Aes::Aes(const std::string& _key)`

	* `_key`: key 
	
	对 key 的长度没有限制

3.  `Aes::Aes(const std::string& _raw256key, Aes::raw256keysetting);` 和 `Aes::Aes(const std::vector<uint8_t>& _raw256key, Aes::raw256keysetting);`

	* `_raw256key`： 256bit 的 key
	* `Aes::raw256keysetting`: 哑元, 用来重载构造函数。

	如果 `_raw256key` 的 size() 不等于 32 (即 256 bit)，在程序运行时会引发断言错误 ：`assert(_raw256key.size() == aes_key_len)`

	用法见 [demo](./demo.md#aes)


	