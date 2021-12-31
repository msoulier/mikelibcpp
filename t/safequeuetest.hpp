#ifndef SAFEQUEUETEST_H
#define SAFEQUEUETEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mlogger.hpp"
#include "mqueue.hpp"

class SafeQueueTest : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE( SafeQueueTest );
    CPPUNIT_TEST( testSimple );
    CPPUNIT_TEST( testQueueLimits );
    CPPUNIT_TEST_SUITE_END();
public:
    void setUp();
    void tearDown();

    void testSimple();
    void testQueueLimits();

private:
    SafeQueue<int> *m_queue;
};

#endif
