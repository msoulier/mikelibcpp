#ifndef SAFEQUEUETEST_H
#define SAFEQUEUETEST_H

#include "mqueue.hpp"

class SafeQueueTest
{
public:
    void setUp();
    void tearDown();

    int testSimple();
    int testQueueLimits();
    int testQueueThreads();

private:
    SafeQueue<int> *m_queue;
};

#endif
