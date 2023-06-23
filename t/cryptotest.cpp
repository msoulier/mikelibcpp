#include <thread>
#include <iostream>

#include "cryptotest.hpp"
#include "mutil.h"

void CryptoTest::testBase64(void) {
    std::string plaintext{ "this is a simple string" };
    char *b64 = base64_encode(plaintext.c_str(), plaintext.size());
    CPPUNIT_ASSERT( b64 != NULL );
    std::string b64string(b64);
    char *decoded = base64_decode(b64string.c_str(), b64string.size());
    CPPUNIT_ASSERT( decoded != NULL );
    std::string decoded_string(decoded);
    CPPUNIT_ASSERT( decoded_string == plaintext );
}

void CryptoTest::testEncryptDecryptAES(void) {
    std::string key{ "shhhdon'tttellanyone" };
    std::string iv{ "123456789" };
    std::string plaintext("this is my secret password");

    AESEncryptor encryptor(key, iv);

    std::string encrypted = encryptor.encrypt(plaintext);

    CPPUNIT_ASSERT( !encrypted.empty() );

    std::string decrypted = encryptor.decrypt(encrypted);

    CPPUNIT_ASSERT( !decrypted.empty() );

    std::cout << "plaintext: " << plaintext << std::endl;
    std::cout << "decrypted: " << decrypted << std::endl;

    CPPUNIT_ASSERT( decrypted.size() == plaintext.size() );

    CPPUNIT_ASSERT( decrypted == plaintext );
}
