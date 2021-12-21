#include <iostream>

#include "mlogger.hpp"
#include "mqueue.hpp"

void testsafequeue(void) {
    std::cout << "Testing SafeQueue with ints" << std::endl;

    SafeQueue<int> queue(10);
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
    std::cout << "Testing MLogger" << std::endl;

    MLogger logger;
    logger.setDefaults();

    logger.info() << "Hello at info level" << std::endl;
}

int main(void) {
    std::cout << std::endl;
    testlogger();

    std::cout << std::endl;
    testsafequeue();

    return 0;
}
