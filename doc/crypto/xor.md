crypto/xor_crypto.h

_____

# crypto::Xor

```cpp
class crypto::Xor : public crypto::Encrypt;
```

使用`异或`(`xor`/`^`) 算法的加解密类

### crypto::Xor 类摘要

```cpp
class Xor : public Encrypt {
public:
    Xor(const uint8_t* _key, size_t _key_len, bool _is_synchro = false); 
    Xor(const Xor&) = delete;
    Xor& operator= (const Xor&) = delete;
    
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
```

构造函数 `Xor(const uint8_t* _key, size_t _key_len, bool _is_synchro = false);` 

1. `_key`: key 的首地址

2. `_key_len`: key 的长度

3. `_is_synchro`: 是否执行同步模式，可以并行加解密，默认值 `false`, 即默认不启用同步模式。

	同步模式开启后，就意味着 该类的对象在完成一次加密操作之后，必须紧接着进行解密操作；换言之，不能连续两次加密，或连续两次解密，否则将得不到正确的结果。