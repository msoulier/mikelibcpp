#include <thread>
#include <iostream>
#include <vector>
#include <stdio.h>

#include "cryptotest.hpp"
#include "mutil.h"

void CryptoTest::testBase64(void) {
    std::vector<std::string> inputs{
        "this is an input string",
        "this is a simple string",
        "foo",
        "a very long string that hopefully comes back from encoding",
        "should I have some binary data here?",
        "bar"
        };

    Base64Encoder encoder;

    for (auto input : inputs) {
        printf("b64 encoding '%s'\n", input.c_str());
        std::string encoded = encoder.encode(input);
        printf("b64 encoded: %s\n", encoded.c_str());
        std::string decoded = encoder.decode(encoded);
        printf("b64 decoded to '%s'\n", decoded.c_str());
        CPPUNIT_ASSERT( decoded == input );
    }
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
