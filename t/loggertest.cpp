#include <stdio.h>

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
    CPPUNIT_ASSERT( m_logger.getLevel() == MLoggerVerbosity::info );
    int count = 0;
    // Done with getHandlers to test that method.
    std::cout << "handlers:" << std::endl;
    auto vec = m_logger.getHandlers();
    for (auto handler : vec) {
        count++;
        std::cout << "    " << handler->print() << std::endl;
    }
    CPPUNIT_ASSERT( count == 1 );
}
#pragma GCC diagnostic pop

// Test file handler.
void LoggerTest::testFileHandler(void) {
    m_logger.clearHandlers();

	m_logger.addHandler<MLoggerFileHandler>("/tmp/logfile.log",
		500 * 1024 * 1024, 300, true);

    // Should be the only handler.
    int count = 0;
    std::cout << "handlers:" << std::endl;
    for (auto handler : m_logger.getHandlers()) {
        count++;
        std::cout << "    " << handler->print() << std::endl;
    }
    CPPUNIT_ASSERT( count == 1 );

    m_logger.info() << "This is a log entry" << std::endl;
    m_logger.warn() << "And this is another one" << std::endl;

    m_logger.clearHandlers();

    // Are the lines in the file?
    FILE *logfile = fopen("/tmp/logfile.log", "r");
    CPPUNIT_ASSERT( logfile != NULL );

    count = 0;
    if (logfile != NULL) {
        char buffer[1024];
        for (;;) {
            if (fgets(buffer, 1024, logfile) == NULL) {
                break;
            }
            count++;
            printf("%d: %s", count, buffer);
        }
    }
    fclose(logfile);
    CPPUNIT_ASSERT( count == 2 );
}
