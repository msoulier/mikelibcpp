#include <iostream>

#include "mlogger.hpp"
#include "mqueue.hpp"

void testsafequeue(void) {
    std::cout << "Testing SafeQueue with ints" << std::endl;

    SafeQueue<int> queue(10);

    std::cout << "maxsize of queue is " << queue.maxsize() << std::endl;

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);
    queue.enqueue(5);

    std::cout << "SafeQueue size is " << queue.size() << std::endl;

    std::cout << "dequeue " << queue.dequeue() << std::endl;
    std::cout << "dequeue " << queue.dequeue() << std::endl;
    std::cout << "dequeue " << queue.dequeue() << std::endl;
    std::cout << "dequeue " << queue.dequeue() << std::endl;
    std::cout << "dequeue " << queue.dequeue() << std::endl;

    std::cout << "SafeQueue size is " << queue.size() << std::endl;
}

void testlogger(void) {
    std::cout << "Testing MLogger iostream interface" << std::endl;

    MLogger logger;
    logger.setDefaults();

    logger.info() << "Hello at info level" << std::endl;

    std::cout << std::endl;
    std::cout << "Testing MLogger printf interface" << std::endl;

    logger.info("Hello at %s level", "info");
}

int main(void) {
    std::cout << std::endl;
    testlogger();

    std::cout << std::endl;
    testsafequeue();

    return 0;
}
