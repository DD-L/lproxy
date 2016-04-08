crypto/encryptor.h

-----

# crypto::Encrypt

```cpp
class crypto::Encrypt;
```

所有 `crypto` 加解密类型的接口类


### Encrypt 类摘要

```cpp
class Encrypt {
public:
    Encrypt(void) = default;
    Encrypt(const Encrypt&) = delete;
    Encrypt& operator= (const Encrypt&) = delete;
    virtual ~Encrypt() {}

    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len) = 0;
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len) = 0;
};
```

所有 `crypto` 加解密类型，都继承自 `Encrypt`


# Encryptor

```cpp
class crypto::Encryptor;
```

加密器， 所有的 `crypto` 加解密对象的加密和解密操作，都可以通过它来完成。


### crypto::Encryptor 类摘要

```cpp
class Encryptor {
public:
    explicit Encryptor(Encrypt* _encrypt);
    virtual ~Encryptor() {}

    std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);

    std::string& encrypt(std::string& dest, const char* src, size_t src_len);
    std::string& decrypt(std::string& dest, const char* src, size_t src_len);

    std::string& encrypt(std::string& dest, 
            const uint8_t* src, size_t src_len);
    std::string& decrypt(std::string& dest, 
            const uint8_t* src, size_t src_len);
private:
    std::shared_ptr<Encrypt> crypto;
};
```