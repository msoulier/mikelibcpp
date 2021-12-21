#include <iostream>

#include "mlogger.hpp"

int main(void) {
    MLogger logger;
    logger.setDefaults();

    logger.info() << "Hello at info level" << std::endl;

    return 0;
}
