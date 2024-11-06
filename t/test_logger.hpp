#ifndef LOGGERTEST_H
#define LOGGERTEST_H

#include "mlogger.hpp"

class LoggerTest
{
public:
    void setUp();
    void tearDown();

    int testSimple();
    int testFileHandler();

private:
    MLogger m_logger;
};

#endif
