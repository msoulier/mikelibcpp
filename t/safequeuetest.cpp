#include "safequeuetest.hpp"

void SafeQueueTest::setUp() {
    m_queue = new SafeQueue<int>(10);
}

void SafeQueueTest::tearDown() {
    delete m_queue;
}

void SafeQueueTest::testSimple(void) {
    m_queue->enqueue(1);
    m_queue->enqueue(2);
    m_queue->enqueue(3);
    m_queue->enqueue(4);
    m_queue->enqueue(5);
    CPPUNIT_ASSERT(m_queue->size() == 5);
    int item;
    CPPUNIT_ASSERT(m_queue->dequeue(item));
    CPPUNIT_ASSERT(item == 1);
    CPPUNIT_ASSERT(m_queue->dequeue(item));
    CPPUNIT_ASSERT(item == 2);
    CPPUNIT_ASSERT(m_queue->dequeue(item));
    CPPUNIT_ASSERT(item == 3);
    CPPUNIT_ASSERT(m_queue->dequeue(item));
    CPPUNIT_ASSERT(item == 4);
    CPPUNIT_ASSERT(m_queue->dequeue(item));
    CPPUNIT_ASSERT(item == 5);
    CPPUNIT_ASSERT(! m_queue->dequeue(item));
}

void SafeQueueTest::testQueueLimits(void) {
    int input[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (int i = 0; i < 13; i++) {
        if (i < 10) {
            CPPUNIT_ASSERT(m_queue->enqueue(input[i], false));
        } else {
            CPPUNIT_ASSERT(!m_queue->enqueue(input[i], false));
        }
    }
    for (int i = 0; i < 13; i++) {
        int item;
        if (i < 10) {
            CPPUNIT_ASSERT(m_queue->dequeue(item, false));
        } else {
            CPPUNIT_ASSERT(!m_queue->dequeue(item, false));
        }
    }
}

CppUnit::TestSuite *SafeQueueTest::suite()
{
    CppUnit::TestSuite *suiteOfTests = new CppUnit::TestSuite("SafeQueueTest");
    suiteOfTests->addTest( new CppUnit::TestCaller<SafeQueueTest>(
                                   "testSimple",
                                   &SafeQueueTest::testSimple ) );
    suiteOfTests->addTest( new CppUnit::TestCaller<SafeQueueTest>(
                                   "testQueueLimits",
                                   &SafeQueueTest::testQueueLimits ) );
    return suiteOfTests;
}
