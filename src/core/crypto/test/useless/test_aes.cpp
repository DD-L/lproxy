/*************************************************************************
	> File Name: test_aes.cpp
	> Author: D_L
	> Mail: deel@d-l.top
	> Created Time: 2015/11/7 9:30:53
 ************************************************************************/

#include <iostream>
#include "aes.h"

int aes() {
    using namespace std;
    cout << "AES Parameters: " << endl;
    cout << "Algorithm name : " << CryptoPP::AES::StaticAlgorithmName() << endl;

    // CryptoPP::AES::BLOCKSIZE = 16
    cout << "Block size     : " << CryptoPP::AES::BLOCKSIZE << endl;
    // CryptoPP::AES::MIN_KEYLENGTH = 16
    cout << "Min key length : " << CryptoPP::AES::MIN_KEYLENGTH << endl;
    // CryptoPP::AES::MAX_KEYLENGTH = 32
    cout << "Max key length : " << CryptoPP::AES::MAX_KEYLENGTH << endl;
    // CryptoPP::AES::DEFAULT_KEYLENGTH = 16
    cout << "AES::DEFAULT_KEYLENGTH = " << CryptoPP::AES::DEFAULT_KEYLENGTH << endl;

    CryptoPP::AESEncryption aesEncryptor; //加密器 

    // 密钥 CryptoPP::AES::DEFAULT_KEYLENGTH = 16
    //unsigned char aesKey[CryptoPP::AES::DEFAULT_KEYLENGTH] = "123456789";  
    unsigned char aesKey[33] = "4C9E7E7B14F9FFC772962619B05A21A0";  
    //要加密的数据块
    //unsigned char inBlock[CryptoPP::AES::BLOCKSIZE] = "this is a test."; 
    unsigned char inBlock[CryptoPP::AES::BLOCKSIZE] = {0}; 
    unsigned char outBlock[CryptoPP::AES::BLOCKSIZE]; //加密后的密文块
    unsigned char xorBlock[CryptoPP::AES::BLOCKSIZE] = {0};
    memset( xorBlock, 0, CryptoPP::AES::BLOCKSIZE );


    // 设定加密密钥
    //aesEncryptor.SetKey( aesKey, CryptoPP::AES::DEFAULT_KEYLENGTH );
    aesEncryptor.SetKey( aesKey, 32 );
    
    // 加密
    aesEncryptor.ProcessAndXorBlock( inBlock, xorBlock, outBlock );

    //以16进制显示加密后的数据
    for( int i=0; i<16; i++ ) { 
        cout << hex << (int)outBlock[i] << " ";
    }
    cout << endl;

    // 解密
    CryptoPP::AESDecryption aesDecryptor;
    unsigned char plainText[CryptoPP::AES::BLOCKSIZE];
    //aesDecryptor.SetKey( aesKey, CryptoPP::AES::DEFAULT_KEYLENGTH );
    aesDecryptor.SetKey( aesKey, 32 );

    aesDecryptor.ProcessAndXorBlock( outBlock, xorBlock, plainText );

    for( int i=0; i<16; i++ ) { 
        cout << hex << (int)plainText[i] << " ";
    }
    cout << endl;
    cout << plainText << endl;


    return 0;
}

#include "hex.h"
#ifndef CRYPTOPP_ENABLE_NAMESPACE_WEAK
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#endif
#include "md5.h"
void md5() {
    std::cout << sizeof (byte) << std::endl;
    std::cout << CryptoPP::Weak::MD5::DIGESTSIZE << std::endl;

    CryptoPP::Weak::MD5 hash;
    byte digest[ CryptoPP::Weak::MD5::DIGESTSIZE ];
    std::string message = "abcdefghijklmnopqrstuvwxyz";

    hash.CalculateDigest( digest, (byte*) message.c_str(), message.length() );

    CryptoPP::HexEncoder encoder;
    std::string output;
    encoder.Attach( new CryptoPP::StringSink( output ) );
    encoder.Put( digest, sizeof(digest) );
    encoder.MessageEnd();

    std::cout << output << std::endl;
    // 32 字节 / 大写 strlen(output) = 32
}

int main() {
    //md5();
    aes();
    return 0;
}
