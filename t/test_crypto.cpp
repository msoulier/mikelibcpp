#include <thread>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <assert.h>

#include "test_crypto.hpp"
#include "mutil.h"

int CryptoTest::testSha1(void) {
    std::basic_string<unsigned char> input =
        (unsigned char*)"this is an input string";
    std::string digest("014eaac6f30e3957f6f6b60a5378761e1634dde5");

    SHAEncoder encoder;

    std::basic_string<unsigned char> output = encoder.quickdigest(input);
    char *hex = tohex(output.c_str(), output.size());
    std::string hex_str(hex, hex+output.size()*2);

    std::cout << "hex_str is " << hex_str << std::endl;

    assert( hex_str == digest );

    return 0;
}

int CryptoTest::testBase64(void) {
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
        std::basic_string<unsigned char> indata(input.begin(), input.end());
        printf("b64 encoding '%s', size %ld\n", input.c_str(), indata.size());
        printf("indata is %ld bytes long\n", indata.size());
        std::string encoded = encoder.encode(indata);
        printf("b64 encoded: %s\n", encoded.c_str());
        std::basic_string<unsigned char> decoded = encoder.decode(encoded);

        for (unsigned long int i = 0; i < decoded.size(); ++i) {
            uint8_t a = indata[i];
            uint8_t b = decoded[i];
            assert( a == b );
        }

        printf("decoded size is %ld bytes\n", decoded.size());
        std::string decoded_s((char *)decoded.data(), decoded.size());
        printf("b64 decoded: '%s', size %ld\n", decoded_s.c_str(), decoded_s.size());
        if (decoded_s == input) {
            printf("match\n");
        } else {
            printf("no match\n");
        }
        //assert( decoded_s == input );
    }

    return 0;
}

int CryptoTest::testEncryptDecryptAES(void) {
    std::basic_string<unsigned char> key = (unsigned char*)"shhhdon'tttellanyone";
    std::basic_string<unsigned char> iv = (unsigned char*)"123456789";
    std::string plaintext("this is my secret password");

    AESEncryptor encryptor(key, iv);

    std::basic_string<unsigned char> encrypted = encryptor.encrypt(plaintext);

    assert( !encrypted.empty() );

    std::string decrypted = encryptor.decrypt(encrypted);

    assert( !decrypted.empty() );

    std::cout << "plaintext: " << plaintext << std::endl;
    std::cout << "decrypted: " << decrypted << std::endl;

    assert( decrypted.size() == plaintext.size() );

    assert( decrypted == plaintext );

    return 0;
}

int
main(void) {
    CryptoTest test;
    int rv = test.testBase64() &&
        test.testEncryptDecryptAES() &&
        test.testSha1();

    return rv;
}
