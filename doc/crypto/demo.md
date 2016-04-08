# crypto demo

### xor

异或

```cpp
#include <crypto/xor_crypto.h>
#include <cstdio>   // for printf
#include <cstring>  // for strlen

void test_xor(void) {
	using namespace crypto;
	const char* key = "hello crypto!";
	uint8_t buffer[1024] = "This is a test ...";
	std::vector<uint8_t> cipher, recovered;
	// std::string 也可以
	
	Encryptor xor_(new Xor((const uint8_t*)key, strlen(key))); 
	
	// 加密
	xor_.encrypt(cipher, buffer, 1024);	
	for (auto& i : cipher) {
		printf("%x ", (int)i);
	}
	printf("\n");

	// 解密
	xor_.encrypt(recovered, &cipher[0], cipher.size());
	for (auto& i : recovered) {
		printf("%x ", (int)i);
	}
	printf("\n");
}
```

默认支持并行加解密

如果要开启同步加解密模式，只需将 `new Xor((const uint8_t*)key, strlen(key))` 改成 `new Xor((const uint8_t*)key, strlen(key), true)`


### rc4

```cpp
#include <crypto/rc4_crypto.h>
#include <cstdio>   // for printf
#include <cstring>  // for strlen
#include <assert.h> // for assert
void test_rc4(void) {
    using namespace crypto;
    const char* key = "hello crypto!";
    uint8_t buffer[1024] = "This is a test ...";
    std::vector<uint8_t> cipher, recovered;
    // std::string 也可以

    Encryptor rc4(new Rc4((const uint8_t*)key, strlen(key))); 

    // 加密
    rc4.encrypt(cipher, buffer, 1024);
    for (auto& i : cipher) {
        printf("%x ", (int)i);
    }
    printf("\n\n");

    // 解密
    rc4.decrypt(recovered, &cipher[0], cipher.size());

    assert(recovered.size() == 1024);

    for (int i = 0; i < 1024; ++i) {
        printf("%x ", (int)recovered[i]);
        assert(recovered[i] == buffer[i]);
    }
    printf("\n")
}
```
内置同步机制，无法并行加解密

### aes

AES-256 CTR mode

```cpp
#include <crypto/aes_crypto.h>
#include <cstdio>   // for printf
#include <assert.h> // for assert
void test(void) {
    using namespace crypto;
    // key 的长度是任意的
    const char* key = "hello crypto!";

    const std::size_t buffer_size = 1023; 
    uint8_t buffer[buffer_size] = {0};

    for (std::size_t i = 0; i < buffer_size; ++i) {
        static char flag = 'A';
        if ((i % 10) == 0) {
            buffer[i] = flag++;
            if (flag >= 'Z') flag = 'A';
        }
        else {
            buffer[i] = i % 10 + '0';
        }
    }

    std::string cipher, recovered;
    // std::vector<uint8_t> 也可以

    Encryptor aes(new Aes(std::string(key)));

    // 加密
    aes.encrypt(cipher, buffer, buffer_size);   
    for (auto& i : cipher) {
        printf("%x ", (uint8_t)i);
    }
    printf("\n\n");

    // 解密
    aes.decrypt(recovered, &cipher[0], cipher.size());

    assert(recovered.size() == buffer_size);
    for (std::size_t i = 0; i < buffer_size; ++i) {
        printf("%x ", (uint8_t)recovered[i]);
        assert(recovered[i] == buffer[i]);
    }
    printf("\n");
}
```

使用 raw-256bit 的 key 的示例：

```cpp
#include <crypto/aes_crypto.h>
#include <cstdio>   // for printf
#include <assert.h> // for assert
void test_aes(void) {
	using namespace crypto;
	
    const std::size_t buffer_size = 155;
    uint8_t buffer[buffer_size] = "abcdefghi"; 
    
    uint8_t key[32] = {
		0x23, 0xa3, 0x47, 0x78, 0x9a, 0x45, 0x87,
		0x9a, 0x45, 0x87, 0x23, 0xa3, 0x47, 0x78,
		0x78, 0x9a, 0x45, 0x9a, 0x45, 0x23, 0xa3,
		0x45, 0x87, 0x23, 0x00, 0x9a, 0x45, 0x01
	};
	
	const std::string raw_key(key, key + 32);
	// std::vector<uint8_t> 也可以

	assert(raw_key.size() == (256 / 8));

	Encryptor aes(new Aes(raw_key, Aes::raw256keysetting()));

	std::string cipher, recovered;
	// std::vector<uint8_t> 也可以
	
	// 加密
	aes.encrypt(cipher, buffer, buffer_size);	
	for (auto& i : cipher) {
		printf("%x ", (uint8_t)i);
	}
	printf("\n\n");

	// 解密
	aes.encrypt(recovered, &cipher[0], cipher.size());

    assert(recovered.size() == buffer_size);
    for (std::size_t i = 0; i < buffer_size; ++i) {
        printf("%x ", (uint8_t)recovered[i]);
        assert(recovered[i] == buffer[i]);
    }
    printf("\n");
}
```

crypto::Aes 可以并行加解密


### rsa 

RSA/OAEP-MGF1(SHA-1) 非对称加解密

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

key 的 模长越大越安全，生成 key 所花费的时间越长。

```cpp
#include <crypto/rsa_crypto.h>
#include <cstdio>   // for printf
#include <assert.h> // for assert
#include <iostream> // for std::cout std::endl
void test_rsa(void) {
	using namespace crypto;
	uint8_t buffer[] = "this is a test..ffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"
        "fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff";

	const std::size_t buffer_size = sizeof(buffer) / sizeof(buffer[0]);
	
	// 生成 模长 1024bit 的 key
	RsaKey rsakey(RsaKey::bit1024);
	
	// 公钥 用来加密 （std::string 类型）
	std::cout << "\npublic key: " << rsakey.publicKeyHex() << std::endl;
	// 私钥 用来解密  (std::string 类型)
	std::cout << "\nprivate key: " << rsakey.privateKeyHex() << std::endl;

	Encryptor rsa(new Rsa(rsakey));
	
	std::vector<uint8_t> cipher, recovered;
	// std::string 也可以
	
	// 加密
	rsa.encrypt(cipher, buffer, buffer_size);
	printf("\n");
	for (auto& i : cipher) {
		printf("%x ", (int)i);
	}
	printf("\n\n");

	// 解密
	rsa.decrypt(recovered, &cipher[0], cipher.size());
    assert(recovered.size() == buffer_size);
    for (std::size_t i = 0; i < buffer_size; ++i) {
		printf("%c", recovered[i]);
        assert(recovered[i] == buffer[i]);
	}
	printf("\n");

	//////////////////////////////////////////////////////

	// 用 key 模长 和 公钥、私钥 构造 Rsa 对象
	Rsa rsa_p(rsakey.keySize(), rsakey.publicKeyHex(), rsakey.privateKeyHex());
	
	// 或
   	//Rsa rsa_p(RsaKey::bit1024, // 或 rsakey.keySize()
	//			&(rsakey.publicKeyHex()[0]), rsakey.publicKeyHex().size(),
	//			&(rsakey.privateKeyHex()[0]), rsakey.privateKeyHex().size());

	//此时的 rsa_p 对象拥有完整的 公钥 和 私钥， 可以完成 加密任务 和 解密任务
    std::vector<uint8_t> cipher_p, recovered_p;
	rsa_p.encrypt(cipher_p, buffer, buffer_size);
	rsa_p.decrypt(recovered_p, &cipher_p[0], cipher_p.size());

	// 用 key 模长 和 公钥 构造 Rsa 对象
	Rsa rsa_pub(rsakey.keySize(), rsakey.publicKeyHex());
	
	// 或
	// Rsa rsa_pub(RsaKey::bit1024, // 或 rsakey.keySize()
	//			&(rsakey.publicKeyHex()[0]), rsakey.publicKeyHex().size();
	
	// 此时的rsa_pub 对象只拥有 公钥，只能完成 加密任务。
    std::vector<uint8_t> cipher_pub, recovered_pub;
	rsa_pub.encrypt(cipher_pub, buffer, buffer_size);
	try {
		// 在执行时会抛出一个“解密时异常”
		rsa_pub.decrypt(recovered_pub, &cipher_pub[0], cipher_pub.size()); 
	}
	catch(std::exception& e) {
		std::cout << e.what() << std::endl;
	}

	////////////////////////////////////
	// rsa_p 和 rsa_pub 都是用 key 的字符串形式构造的， 
	// Rsa 还可以用 crypto::RsaKey::Pair 来构造， 见 “详细文档”
}
```
crypto::Rsa 可以并行加解密

注意 RSA 用 同一公钥加密出的密文，密文和密文之间会不一样。

### base64

RFC2045 标准规定:
> The encoded output stream must be represented in lines of no more than 76 characters each.

```cpp
#include <crypto/base64_crypto.h>
#include <iostream> // for std::cout std::endl
void test_base64(void) {
	using namespace crypto;
	const std::string plain = "hello crypto!";
	std::string cipher, recovered;
	// 也可以是 std::vector<uint8_t>

	Encryptor base64(new Base64());
	// 默认不执行 RFC2045 的 76 字符换行的规定。
	// 如果 要严格执行 RFC2045 的 76 字符换行的规定
	// 可以这样构造 Base64 对象：
	// new Base64(true)

	// 加密
	base64.encrypt(cipher, plain.c_str(), plain.size());
	
    for (auto& i : cipher) {
        std::cout << i;
    }
    std::cout << std::endl;

	// 解密
	base64.decrypt(recovered, &cipher[0], cipher.size());
    for (auto& i : recovered) {
        std::cout << i;
    }
    std::cout << std::endl;

```

crypto::Base64 支持并行加解密

### md5

```cpp
#include <crypto/md5_crypto.h>
#include <algorithm> // for std::copy
#include <iterator>  // for std::ostream_iterator
#include <assert.h>  // for assert
#include <iostream>  // for std::cout

void test_md5(void) {
	using namespace crypto;
	const std::string src = "hello crypto!";
	std::string cipher; // std::vector<uint8_t> 也行

	Encryptor md5(new Md5());

	// 加密
	md5.encrypt(cipher, src.c_str(), src.size());
	// or md5.encrypt(cipher, &src[0], src.size());

	std::copy(cipher.begin(), cipher.end(),
            std::ostream_iterator<char>(std::cout));

    std::cout << "\n";
	assert(cipher == std::string("4C9E7E7B14F9FFC772962619B05A21A0"));


    try {
        std::vector<uint8_t> recovered;
		// md5 加密是不可逆的，运行时会抛出“解密时异常”
        md5.decrypt(recovered, (const uint8_t*)&cipher[0], cipher.size());
    }
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

```

注意 cryptp::Md5 加密的结果是 32 个字节长度，并且字母都为大写

用 python 描述 crypto::Md5 加密的结果 是这样的：

```python
import hashlib
src = "hello crypto!"
hashlib.md5(src.encode()).hexdigest().upper()

'4C9E7E7B14F9FFC772962619B05A21A0'
```

### crypto 异常处理


* `CryptoException` 
	
	crypto 加解密异常， 继承自 `std::exception`

* `EncryptException`

	crypto 加密时异常， 继承自 `CryptoException`

* `DecryptException`

	crypto 解密时异常， 继承自 `CryptoException`
 