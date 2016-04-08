crypto/md5_crypto.h

----------

# crypto::Md5

```cpp
class crypto::Md5 : public crypto::Encrypt;
```

采用 md5 加密算法的加密类， 该类依赖 `cryptopp`

注意，md5 是不可逆加密算法，所以试图调用该类的解密操作，会引发“解密时异常”`DecryptException`。用法见 [demo](./demo.md#md5)

该类的加密算法，用 python 语言描述是：

```python
import hashlib
src = "hello crypto!"
hashlib.md5(src.encode()).hexdigest().upper()

'4C9E7E7B14F9FFC772962619B05A21A0'
```

即， 加密的结果是 32 个字节长度，并且字母都为大写。

### crypto::Md5 类摘要

```cpp
class Md5 : public Encrypt {
public:
    Md5(const Md5&) = delete;
    Md5& operator= (const Md5&) = delete;
    Md5(void);
    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
private:
    static const size_t md5_len = 32;
}; // class crypto::Md5
```