#include <thread>
#include <iostream>

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

bool producer_success = false;
bool consumer_success = false;

void producer(SafeQueue<int> *queue) {
    int number = 1000;
    for (int i = 0; i < number; i++) {
        queue->enqueue(i);
    }
    producer_success = true;
}

void consumer(SafeQueue<int> *queue) {
    int number = 1000;
    int item;
    for (int i = 0; i < number; i++) {
        queue->dequeue(item);
    }
    consumer_success = true;
}

// Test with a producer and a consumer thread.
void SafeQueueTest::testQueueThreads(void) {
    std::thread prod(producer, m_queue);
    std::thread cons(consumer, m_queue);

    prod.join();
    cons.join();

    CPPUNIT_ASSERT( producer_success );
    CPPUNIT_ASSERT( consumer_success );
}
