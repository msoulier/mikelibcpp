#ifndef CRYPTOTEST_H
#define CRYPTOTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mcrypto.hpp"

class CryptoTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE( CryptoTest );
    CPPUNIT_TEST( testSimple );
    CPPUNIT_TEST_SUITE_END();
public:
    void testSimple();
};

#endif
