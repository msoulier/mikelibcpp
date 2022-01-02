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
    auto vec = m_logger.getHandlers();
    for (auto it : vec) {
        count++;
    }
    CPPUNIT_ASSERT(count == 1);
}
#pragma GCC diagnostic pop
