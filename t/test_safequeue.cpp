#include <thread>
#include <iostream>
#include <assert.h>

#include "test_safequeue.hpp"

void SafeQueueTest::setUp() {
    m_queue = new SafeQueue<int>(10);
}

void SafeQueueTest::tearDown() {
    delete m_queue;
}

int SafeQueueTest::testSimple(void) {
    m_queue->enqueue(1);
    m_queue->enqueue(2);
    m_queue->enqueue(3);
    m_queue->enqueue(4);
    m_queue->enqueue(5);
    assert(m_queue->size() == 5);
    int item;
    assert(m_queue->dequeue(item));
    assert(item == 1);
    assert(m_queue->dequeue(item));
    assert(item == 2);
    assert(m_queue->dequeue(item));
    assert(item == 3);
    assert(m_queue->dequeue(item));
    assert(item == 4);
    assert(m_queue->dequeue(item));
    assert(item == 5);
    assert(! m_queue->dequeue(item));
    return 0;
}

int SafeQueueTest::testQueueLimits(void) {
    int input[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (int i = 0; i < 13; i++) {
        if (i < 10) {
            assert(m_queue->enqueue(input[i], false));
        } else {
            assert(!m_queue->enqueue(input[i], false));
        }
    }
    for (int i = 0; i < 13; i++) {
        int item;
        if (i < 10) {
            assert(m_queue->dequeue(item, false));
        } else {
            assert(!m_queue->dequeue(item, false));
        }
    }
    return 0;
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
int SafeQueueTest::testQueueThreads(void) {
    std::thread prod(producer, m_queue);
    std::thread cons(consumer, m_queue);

    prod.join();
    cons.join();

    assert( producer_success );
    assert( consumer_success );

    return 0;
}

int
main(void) {
    SafeQueueTest qtest;
    int rv = 0;

    qtest.setUp();
    rv = qtest.testSimple();
    qtest.tearDown();

    qtest.setUp();
    rv |= qtest.testQueueLimits();
    qtest.tearDown();

    qtest.setUp();
    rv |= qtest.testQueueThreads();
    qtest.tearDown();

    return rv;
}