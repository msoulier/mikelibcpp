#include <iostream>

#include "mlogger.hpp"
#include "mqueue.hpp"

void testsafequeuelimits(void) {
    SafeQueue<int> queue(10);
    int input[13] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
    for (int i = 0; i < 13; i++) {
        std::cout << "enqueueing " << input[i] << ": ";
        if (queue.enqueue(input[i], false)) {
            std::cout << "successful" << ": ";
            if (i < 10) {
                std::cout << "pass" << std::endl;
            } else {
                std::cout << "fail" << std::endl;
            }
        } else {
            std::cout << "unsuccessful" << ": ";
            if (i >= 10) {
                std::cout << "pass" << std::endl;
            } else {
                std::cout << "fail" << std::endl;
            }
        }
    }
    std::cout << std::endl;
    for (int i = 0; i < 13; i++) {
        std::cout << "dequeueing: ";
        int item;
        if (queue.dequeue(item, false)) {
            std::cout << "successful: " << item << ": ";
            if (i < 10) {
                std::cout << "pass" << std::endl;
            } else {
                std::cout << "fail" << std::endl;
            }
        } else {
            std::cout << "unsuccessful: ";
            if (i >= 10) {
                std::cout << "pass" << std::endl;
            } else {
                std::cout << "fail" << std::endl;
            }
        }
    }
}

void testsafequeuesimple(void) {
    std::cout << "Testing SafeQueue with ints" << std::endl;

    SafeQueue<int> queue(10);

    std::cout << "maxsize of queue is " << queue.maxsize() << std::endl;

    std::cout << "1" << std::endl;
    queue.enqueue(1);
    std::cout << "2" << std::endl;
    queue.enqueue(2);
    std::cout << "3" << std::endl;
    queue.enqueue(3);
    std::cout << "4" << std::endl;
    queue.enqueue(4);
    std::cout << "5" << std::endl;
    queue.enqueue(5);

    std::cout << "SafeQueue size is " << queue.size() << std::endl;

    int item;
    std::cout << "dequeueing 1" << ": ";
    if (queue.dequeue(item)) {
        std::cout << "success: " << item << std::endl;
    } else {
        std::cout << "error" << std::endl;
    }
    std::cout << "dequeueing 2" << ": ";
    if (queue.dequeue(item)) {
        std::cout << "success: " << item << std::endl;
    } else {
        std::cout << "error" << std::endl;
    }
    std::cout << "dequeueing 3" << ": ";
    if (queue.dequeue(item)) {
        std::cout << "success: " << item << std::endl;
    } else {
        std::cout << "error" << std::endl;
    }
    std::cout << "dequeueing 4" << ": ";
    if (queue.dequeue(item)) {
        std::cout << "success: " << item << std::endl;
    } else {
        std::cout << "error" << std::endl;
    }
    std::cout << "dequeueing 5" << ": ";
    if (queue.dequeue(item)) {
        std::cout << "success: " << item << std::endl;
    } else {
        std::cout << "error" << std::endl;
    }

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
    testsafequeuesimple();

    std::cout << std::endl;
    testsafequeuelimits();

    return 0;
}
