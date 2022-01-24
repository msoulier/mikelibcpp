#include "loggertest.hpp"

void LoggerTest::setUp() {
    m_logger.setDefaults();
}

void LoggerTest::tearDown() {
    m_logger.clearHandlers();
}

// Silence unused variable warning - yuk
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
void LoggerTest::testSimple(void) {
    CPPUNIT_ASSERT(m_logger.getLevel() == MLoggerVerbosity::info);
    int count = 0;
    // Done with getHandlers to test that method.
    std::cout << "handlers:" << std::endl;
    auto vec = m_logger.getHandlers();
    for (auto handler : vec) {
        count++;
        std::cout << "    " << handler->print() << std::endl;
    }
    CPPUNIT_ASSERT(count == 1);
}
#pragma GCC diagnostic pop

// Test file handler.
void LoggerTest::testFileHandler(void) {
    m_logger.clearHandlers();
    MLoggerHandler *handler = new MLoggerFileHandler("/tmp/logfile.log",
                                                     500*1024*1024,
                                                     300,
                                                     true);
    m_logger.addHandler(handler);

    // Should be the only handler.
    int count = 0;
    std::cout << "handlers:" << std::endl;
    for (auto handler : m_logger.getHandlers()) {
        count++;
        std::cout << "    " << handler->print() << std::endl;
    }
    CPPUNIT_ASSERT(count == 1);
}
