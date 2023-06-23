#include <thread>
#include <iostream>

#include "cryptotest.hpp"
#include "mutil.h"

void CryptoTest::testSimple(void) {
    std::string plaintext{ "this is a simple string" };
    char *b64 = base64_encode(plaintext.c_str(), plaintext.size());
    CPPUNIT_ASSERT( b64 != NULL );
    std::string b64string(b64);
    char *decoded = base64_decode(b64string.c_str(), b64string.size());
    CPPUNIT_ASSERT( decoded != NULL );
    std::string decoded_string(decoded);
    CPPUNIT_ASSERT( decoded_string == plaintext );
}
