#ifndef STRINGTEST_H
#define STRINGTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mstring.hpp"

class StringTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE( StringTest );
    CPPUNIT_TEST( testBasic );
    CPPUNIT_TEST_SUITE_END();
public:
    void testBasic();
};

#endif
