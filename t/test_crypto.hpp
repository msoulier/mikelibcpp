#ifndef CRYPTOTEST_H
#define CRYPTOTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mcrypto.hpp"

class CryptoTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE( CryptoTest );
    CPPUNIT_TEST( testBase64 );
    CPPUNIT_TEST( testEncryptDecryptAES );
    CPPUNIT_TEST( testSha1 );
    CPPUNIT_TEST_SUITE_END();
public:
    void testBase64();
    void testEncryptDecryptAES();
    void testSha1();
};

#endif
