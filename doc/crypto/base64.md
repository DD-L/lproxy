crypto/base64_crypto.h

----------

# crypto::Base64

```cpp
class crypto::Base64 : public crypto::Encrypt;
```

采用 base64 算法的加解密类, 该类依赖 `cryptopp`

### crypto::Base64 类摘要

```cpp
class Base64 : public Encrypt {
public:
    Base64(const Base64&) = delete;
    Base64& operator= (const Base64&) = delete;
    // LF 编码输出中是否允许包含换行符, 默认不允许包含换行符
    // true: 编码输出包含换行符; false: 编码输出包含换行符  
    Base64(bool allow_LF = false);

    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
private:
    bool LF; // true: 编码输出包含换行符; false: 编码输出包含换行符  
}; // class crypto::Base64

```

RFC2045 标准规定:

> The encoded output stream must be represented in lines of no more than 76 characters each.

所以构造函数被设计成这个样子：`Base64::Base64(bool allow_LF = false);`

`allow_LF` 默认值为 `false`，即默认不执行 RFC2045 标准规定的 “76 字符换行”；如果 `allow_LF` 传入 `true` ，则执行 RFC2045 标准规定的 “76 字符换行”。


