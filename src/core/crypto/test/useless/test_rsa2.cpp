/*************************************************************************
	> File Name:    rsa.cpp
	> Author:       D_L
	> Mail:         deel@d-l.top
	> Created Time: 2015/12/7 8:35:26
 ************************************************************************/

/*
g++ -Wall -g -std=gnu++0x     -I/cygdrive/e/GIT/lproxy/src/core/crypto/test/../../ -I/cygdrive/e/GIT/lproxy/src/core/crypto/test/../..//../../contrib/cryptopp -I.  -c test_rsa2.cpp -o ~tmp/test_rsa2.o

g++ ~tmp/test_rsa2.o /cygdrive/e/GIT/lproxy/src/core/crypto/test/../..//crypto/lib/libcrypto.a /cygdrive/e/GIT/lproxy/src/core/crypto/test/../..//../../contrib/cryptopp/cryptopp/libcryptopp.a -o bin/test_rsa2



cryptopp$ rsa_kgen.exe 61440
Elapsed time for 61140 RSA key: 25654.01s (7 hours, 7 minutes, 34 seconds)
cryptopp$ rsa_kgen.exe 30720
Elapsed time for 30720 RSA key: 2255.30s (37 minutes, 35 seconds)
cryptopp$ rsa_kgen.exe 15360
Elapsed time for 15360 RSA key: 285.05s (4 minutes, 45 seconds)
cryptopp$ rsa_kgen.exe 11776
Elapsed time for 11776 RSA key: 142.52s (2 minutes, 22 seconds)
cryptopp$ rsa_kgen.exe 8192
Elapsed time for 8192 RSA key: 43.08s (43 seconds)
cryptopp$ rsa_kgen.exe 4096
Elapsed time for 4096 RSA key: 0.70s
cryptopp$ rsa_kgen.exe 2048
Elapsed time for 2048 RSA key: 0.09s
cryptopp$ rsa_kgen.exe 1024
Elapsed time for 1024 RSA key: 0.01s
cryptopp$ rsa_kgen.exe 512
Elapsed time for 512 RSA key: 0.00s
cryptopp$ rsa_kgen.exe 256
Elapsed time for 256 RSA key: 0.00s
*/

//#include <iostream>
//using namespace std;

#include "cryptopp/rsa.h"
using CryptoPP::RSA;
using CryptoPP::InvertibleRSAFunction;
using CryptoPP::RSAES_OAEP_SHA_Encryptor;
using CryptoPP::RSAES_OAEP_SHA_Decryptor;

#include "cryptopp/sha.h"
using CryptoPP::SHA1;

#include "cryptopp/filters.h"
using CryptoPP::StringSink;
using CryptoPP::StringSource;
using CryptoPP::PK_EncryptorFilter;
using CryptoPP::PK_DecryptorFilter;

#include "cryptopp/hex.h"

#include "cryptopp/files.h"
using CryptoPP::FileSink;
using CryptoPP::FileSource;

#include "cryptopp/osrng.h"
using CryptoPP::AutoSeededRandomPool;

#include "cryptopp/SecBlock.h"
using CryptoPP::SecByteBlock;

#include "cryptopp/cryptlib.h"
using CryptoPP::Exception;
using CryptoPP::DecodingResult;

#include <string>
using std::string;

#include <exception>
using std::exception;

#include <iostream>
using std::cout;
using std::cerr;
using std::endl;

#include <assert.h>
#include <vector>
int main(int argc, char* argv[])
{
    try
    {
        ////////////////////////////////////////////////
        // Generate keys
        AutoSeededRandomPool rng;

        InvertibleRSAFunction parameters;

        std::cout << "parameters--------------------------\n";
        //parameters.GenerateRandomWithKeySize( rng, 4096 );
        parameters.GenerateRandomWithKeySize( rng, 15360 );
        // public keysize(bit)   cipher(bit) source/block max len(byte)
        // 512                   512         22
        // 1024                  1024        86
        // 2048                  2048        214
        // 4096                  4096        470
        // 8192                  8192        982
        // 11776                 11776       1430
        // 15360                 15360       1878

        // source_max_len_byte = keysize_bit / 8 - 42;


        std::cout << "0--------------------------\n";
        RSA::PrivateKey privateKey( parameters );
        RSA::PublicKey  publicKey( parameters );

        //////////////////////////////////////////////
        //http://marko-editor.com/articles/cryptopp_sign_string/
        std::string str_pubkey;
        publicKey.Save(CryptoPP::HexEncoder(
                    new CryptoPP::StringSink(str_pubkey)).Ref());        
        std::cout << str_pubkey <<  std::endl;
        std::cout << str_pubkey.length() << std::endl;
        std::cout << "1--------------------------\n";
        std::string str_prikey;
        privateKey.Save(CryptoPP::HexEncoder(
                    new CryptoPP::StringSink(str_prikey)).Ref());        
        std::cout << str_prikey <<  std::endl;
        std::cout << str_prikey.length() << std::endl;
        std::cout << "2--------------------------\n";

        ////////////////////////////////////////////////////////
        const std::string& str_prikey2 = str_prikey;
        CryptoPP::RSA::PrivateKey privateKey2;
        privateKey2.Load(CryptoPP::StringSource(str_prikey2, true,
                                         new CryptoPP::HexDecoder()).Ref());


        const std::string& str_pubkey2 = str_pubkey;
        CryptoPP::RSA::PublicKey publicKey2;
        publicKey2.Load(CryptoPP::StringSource(str_pubkey2, true,
                                         new CryptoPP::HexDecoder()).Ref());

        //////////////////////////////////////////////////////////



        //string plain="RSA Encryptionsdjfkjslkfjslkjflksjflsjflksjflksjflksjdlksjdflksjflksjflksjdflksjflksjlksjlskdjflksjsdflsdjflskdfl;skdf;lksdf;lksd;flksd;lfks;ldfk;lsdkf;lsdkf;lsslkdjflksjflkwjeflkjwflkwjeflkjwelkfjwlkefjlwke;jf;;;slkjfl;ksajfl;kajlfkjwalk;fjwal;kfj;WLEFJOIWJFEOWIJEFOIWJEFOIWJEFIOwjfioJWFIJWALFJ;ALWJEF;AWJFEA;JA;skdjfklsjflksjlfkjelkfjslkjflksjflkjslk;fjsa;lkjfl;kajfl;kasjv;alkjvoiajveiwjvoijvoiwvjoijaeviojeovijakvjlakjvlkasjvlksjliejfiwjefiojfioejfioLWIJEFWAJEFOIWJE",
        string plain="RSA Encksjdfkljslkfjsalkdffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffryptionsdjfkjslkfjslkjflksjflsjflksjflksjflksjdlksjdflksjflksjflksjdflksjflksjlksjlskdjflksjsdflsdjflskdfl;skdf;lksdf;lksd;flksd;lfks;ldfk;lsdkf;lsdkf;lsslkdjflksjflkwjeflkjwflkwjeflkjwelkfjwlkefjlwke;jf;;;slkjfl;ksajfl;kajlfkjwalk;fjwal;kfj;WLEFJOIWJFEOWIJEFOIWJEFOIWJEFIOwjfioJWFIJWALFJ;ALWJEF;AWJFEA;JA;skdjfklsjflksjlfkjelkfjslkjflksjflkjslk;fjsa;lkjfl;kajfl;kasjv;alkjvoiajveiwjvoijvoiwvjoijaeviojeovijakvjlakjvlkasjvlksjliejfiwjefiojfioejfioLWIJEFWAJEFOIWJE",
               cipher, recovered;
        //unsigned char plain[1025] = "";


        ////////////////////////////////////////////////
        // Encryption
        //RSAES_OAEP_SHA_Encryptor e( publicKey );
        RSAES_OAEP_SHA_Encryptor e( publicKey2 );


        //AutoSeededRandomPool rng2;
        StringSource( plain, true,
            new PK_EncryptorFilter( rng, e,
                new StringSink( cipher )
            ) // PK_EncryptorFilter
         ); // StringSource

        ////////////////////////////////////////////////
        std::cout << "fff--------------\n" << std::flush;
        std::cout << cipher << std::endl;
        std::cout << cipher.length() << std::endl;
        ////////////////////////////////////////////////

        ////////////////////////////////////////////////
        // Decryption
        //RSAES_OAEP_SHA_Decryptor d( privateKey );
        RSAES_OAEP_SHA_Decryptor d( privateKey2 );

        //AutoSeededRandomPool rng3;
        StringSource( cipher, true,
            new PK_DecryptorFilter( rng, d,
                new StringSink( recovered )
            ) // PK_EncryptorFilter
         ); // StringSource

        assert( plain == recovered );
        std::cout << recovered << std::endl;
    }
    catch( CryptoPP::Exception& e )
    {
        cerr << "Caught Exception..." << endl;
        cerr << e.what() << endl;
        // Caught Exception...
        // BER decode error
    }

	return 0;
}



