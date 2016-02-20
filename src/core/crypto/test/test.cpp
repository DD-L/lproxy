/*************************************************************************
	> File Name:    test.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/11/6 7:31:20
 ************************************************************************/

#include <iostream>
#include <cstring>
#include <assert.h>
#include <algorithm>
#include <iterator>
#include "crypto/xor_crypto.h"
#include "crypto/rc4_crypto.h"
#include "crypto/aes_crypto.h"

void print(const uint8_t* buffer, size_t size) {
    assert(buffer);
    for (size_t i = 0; i < size; ++i) {
        std::cout << (char)(*(buffer + i));
    }
    std::cout << std::endl;
}
void print(const char* buffer, size_t size) {
    print((const uint8_t*)buffer, size);
}
void print(const std::vector<uint8_t>& buffer, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (i >= buffer.size()-1) break;
        std::cout << (char)(buffer[i]);
    }
    std::cout << std::endl;
}
void print_hex(const std::vector<uint8_t>& buffer, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        if (i >= buffer.size()-1) break;
        std::cout << "0x" << std::hex << (int)(buffer[i]) << ' ';
    }
    std::cout << std::endl;
}

void test_xor(void) {
    using namespace crypto;
    const char* key = "hello crypto!";
    uint8_t buffer[1024] = "This is a test ...";
    /*
    uint8_t res1[1024] = {0}, res2[1024] = {0};
    uint8_t res3[1024] = {0}, res4[1024] = {0};
    */
    std::vector<uint8_t> res1, res2;
    std::vector<uint8_t> res3, res4;

    std::cout << buffer << std::endl;
    std::cout << "---" << std::endl; 

    // 默认可以交叉加解密
    Encryptor encryptor(new Xor((const uint8_t*)key, strlen(key))); 
    
    // encrypt
    encryptor.encrypt(res1, buffer, 1024);
    print(res1, 1024);
    std::cout << "---" << std::endl; 

    // decrypt
    encryptor.decrypt(res2, &res1[0], 1024);
    print(res2, 1024);
    std::cout << "---" << std::endl; 

    // encrypt
    encryptor.encrypt(res3, buffer, 1024);
    print(res3, 1024);
    std::cout << "---" << std::endl; 

    // decrypt
    encryptor.decrypt(res4, &res3[0], 1024);
    print(res4, 1024);
    std::cout << "---" << std::endl; 

}

void test_rc4(void) {
    using namespace crypto;
    const char* key = "hello crypto!";
    uint8_t buffer[1024] = "This is a test ...";
    /*
    uint8_t res1[1024] = {0}, res2[1024] = {0};
    uint8_t res3[1024] = {0}, res4[1024] = {0};
    */
    std::vector<uint8_t> res1, res2;
    std::vector<uint8_t> res3, res4;

    
    std::cout << buffer << std::endl;
    std::cout << "---" << std::endl; 

    Encryptor encryptor(new Rc4((const uint8_t*)key, strlen(key))); 
    // encrypt
    encryptor.encrypt(res1, buffer, 1024);
    print(res1, 1024);
    std::cout << "---" << std::endl; 

    // decrypt
    encryptor.decrypt(res2, &res1[0], 1024);
    print(res2, 1024);
    std::cout << "---" << std::endl; 

    // 内置同步机制，无法交叉加解密

    // encrypt
    encryptor.encrypt(res3, buffer, 1024);
    print(res3, 1024);
    std::cout << "---" << std::endl; 

    // decrypt
    encryptor.decrypt(res4, &res3[0], 1024);
    print(res4, 1024);
    std::cout << "---" << std::endl; 

}

void test_aes(void) {
    using namespace crypto;
    const char* key = "hello crypto!";
    const size_t buffer_size = 1024;
    //uint8_t buffer[buffer_size] = "This is a test ...";
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
    /*
    uint8_t res1[buffer_size] = {0}, res2[buffer_size] = {0};
    uint8_t res3[buffer_size] = {0}, res4[buffer_size] = {0};
    */

    std::vector<uint8_t> res1, res2;
    std::vector<uint8_t> res3, res4;

    print(buffer, buffer_size);
    std::cout << "---" << std::endl; 


    Encryptor encryptor(new Aes(std::string(key))); 
    //Encryptor encryptor(new Aes((const uint8_t*)key, strlen(key))); 
    // encrypt
    encryptor.encrypt(res1, buffer, buffer_size);
    print(res1, buffer_size);
    std::cout << "---" << std::endl; 

    // 交叉加解密开始
    // encrypt
    encryptor.encrypt(res3, buffer, buffer_size);
    print(res3, buffer_size);
    std::cout << "---" << std::endl; 

    // decrypt
    encryptor.decrypt(res4, &res3[0], buffer_size);
    print(res4, buffer_size);
    std::cout << "---" << std::endl; 
    // 交叉加解密结束

    // decrypt
    encryptor.decrypt(res2, &res1[0], buffer_size);
    print(res2, buffer_size);
    std::cout << "---" << std::endl; 

}

#include "crypto/rsa_crypto.h"
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
    //const RsaKey rsakey(RsaKey::bit1024);
    RsaKey rsakey(RsaKey::bit1024);

    std::cout << "public key: " << rsakey.publicKeyHex() << std::endl;
    std::cout << "private key: " << rsakey.privateKeyHex() << std::endl;
    // 
    //Encryptor encryptor_0(new Rsa(rsakey.keyPair()));
    Encryptor encryptor_0(new Rsa(rsakey));

    // encryptor_0 加密
    std::vector<uint8_t> cipher;
    encryptor_0.encrypt(cipher, buffer, buffer_size);
    print_hex(cipher, cipher.size());

    // encryptor_0 解密
    std::vector<uint8_t> recovered;
    encryptor_0.decrypt(recovered, &cipher[0], cipher.size());
    print(recovered, recovered.size());

    // 
    // Encryptor encryptor_1(RsaKey::bit1024, "abcdefghijklmnopq...");
    Encryptor encryptor_1(new Rsa(rsakey.keySize(), rsakey.publicKeyHex()));
    // encryptor_1 加密
    std::vector<uint8_t> cipher1;
    encryptor_1.encrypt(cipher1, buffer, buffer_size);
    print_hex(cipher1, cipher1.size());
    // rsa 同一公钥加密出的密文会不一样。
    //assert(cipher1 != cipher);

    // encryptor_0 解密
    std::vector<uint8_t> recovered1;
    encryptor_0.decrypt(recovered1, &cipher1[0], cipher.size());
    print(recovered1, recovered1.size());
    
    assert(recovered1 == recovered);

    try {
        // 
        std::vector<uint8_t> recovered;
        // throw exception
        encryptor_1.decrypt(recovered, &cipher1[0], cipher1.size());
        print(recovered, recovered.size());
    }
    /*
    // #include "except/except.h"
    catch(DecryptException& e) {
        std::cout << e.what() << std::endl;
    }
    catch(CryptoException& e) {
        std::cout << e.what() << std::endl;
    }
    */
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }

}


#include "crypto/base64_crypto.h"
void test_base64(void) {
    using namespace crypto;
    const std::string src = "hello crypto!";
    std::vector<uint8_t> cipher, recovered; 

    // print src
    std::cout << src << std::endl;
    
    Encryptor encryptor(new Base64());
    encryptor.encrypt(cipher, (const uint8_t*)src.c_str(), src.size());

    // print cipher
    std::copy(cipher.begin(), cipher.end(), 
            std::ostream_iterator<char>(std::cout));

    encryptor.decrypt(recovered, &cipher[0], cipher.size());
    
    // print recovered
    std::copy(recovered.begin(), recovered.end(), 
            std::ostream_iterator<char>(std::cout));
    std::cout << std::flush;

    // compare recovered with src
    std::string compared(recovered.begin(), recovered.end());
    assert(src == compared);

}


#include "crypto/md5_crypto.h"
void test_md5(void) {
    using namespace crypto;
    const std::string src = "hello crypto!";
    std::vector<uint8_t> cipher;

    // print src
    std::cout << src << std::endl;

    Encryptor encryptor(new Md5());
    encryptor.encrypt(cipher, (const uint8_t*)src.c_str(), src.size());

    // print cipher
    std::copy(cipher.begin(), cipher.end(),
            std::ostream_iterator<char>(std::cout));

    // compare cipher with "4C9E7E7B14F9FFC772962619B05A21A0"
    std::string compared(cipher.begin(), cipher.end());
    assert(compared == std::string("4C9E7E7B14F9FFC772962619B05A21A0"));
    
    std::cout << std::endl;
    // test decrypt
    try {
        std::vector<uint8_t> recovered;
        encryptor.decrypt(recovered, &recovered[0], recovered.size());
    }
    catch(std::exception& e) {
        std::cout << e.what() << std::endl;
    }
}

int main() {
    using namespace std;

    cout << "\n/*--------test xor--------*/" << endl;
    test_xor();

    cout << "\n/*--------test rc4--------*/" << endl;
    test_rc4();

    cout << "\n/*--------test aes--------*/" << endl;
    test_aes();

    cout << "\n/*--------test rsa--------*/" << endl;
    test_rsa();
    
    cout << "\n/*--------test base64--------*/" << endl;
    test_base64();

    cout << "\n/*--------test md5--------*/" << endl;
    test_md5();

    std::cout << std::endl;
    return 0;
}

