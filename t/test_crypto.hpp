#ifndef CRYPTOTEST_H
#define CRYPTOTEST_H

#include "mcrypto.hpp"

class CryptoTest
{
public:
    int testBase64();
    int testEncryptDecryptAES();
    int testSha1();
};

#endif
