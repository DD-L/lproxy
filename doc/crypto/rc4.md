* crypto/rc4.h
* crypto/rc4_crypto.h

------------------------------

# rc4

### rc4 摘要

```cpp
typedef uint8_t u_char;

struct rc4_state {
	u_char	perm[256];
	u_char	index1;
	u_char	index2;
};

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus

void rc4_init(struct rc4_state *state, const u_char *key, int keylen);
void rc4_crypt(struct rc4_state *state,
		const u_char *inbuf, u_char *outbuf, int buflen);

#ifdef __cplusplus
} // exter "C"
#endif // __cplusplus
```
rc4 被实现为`同步模式`, 不能并行加解密。

`同步模式` 意味着在完成一次加密操作之后，必须紧接着进行解密操作；换言之，不能连续两次加密，或连续两次解密，否则将得不到正确的结果。

# crypto::Rc4

```cpp
class crypto::Rc4 : public crypto::Encrypt;
```
采用 RC4 算法的加解密类

`crypto::Rc4` 依赖上面的 `rc4` ， 即不能并行加解密。

### crypto::Rc4 类摘要

```cpp
class Rc4 : public Encrypt {
public:
    Rc4(const uint8_t* _key, size_t _key_len);
    Rc4(const Rc4&) = delete;
    Rc4& operator= (const Rc4&) = delete;
    
    virtual std::vector<uint8_t>& encrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
    virtual std::vector<uint8_t>& decrypt(std::vector<uint8_t>& dest, 
            const uint8_t* src, size_t src_len);
private:
    const uint8_t*  key;
    size_t          key_len;
    // 内置同步机制，无法交叉加解密(并行加解密)
    rc4_state       en_state, de_state;
};
```
