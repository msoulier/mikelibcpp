#ifndef LOGGERTEST_H
#define LOGGERTEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mlogger.hpp"

class LoggerTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE( LoggerTest );
    CPPUNIT_TEST( testSimple );
    CPPUNIT_TEST( testFileHandler );
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();

    void testSimple();
    void testFileHandler();

private:
    MLogger m_logger;
};

#endif
