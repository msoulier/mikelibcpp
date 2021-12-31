#ifndef SAFEQUEUETEST_H
#define SAFEQUEUETEST_H

#include <cppunit/TestFixture.h>
#include <cppunit/TestAssert.h>
#include <cppunit/extensions/HelperMacros.h>

#include "mlogger.hpp"
#include "mqueue.hpp"

class SafeQueueTest : public CppUnit::TestCase
{
public:
    void setUp();
    void tearDown();

    void testSimple();
    void testQueueLimits();

    static CppUnit::TestSuite *suite();

private:
    SafeQueue<int> *m_queue;
};

#endif
