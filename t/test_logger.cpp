#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "test_logger.hpp"

void LoggerTest::setUp() {
    m_logger.setDefaults();
}

void LoggerTest::tearDown() {
    m_logger.clearHandlers();
}

// Silence unused variable warning - yuk
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
int LoggerTest::testSimple(void) {
    assert( m_logger.getLevel() == MLoggerVerbosity::info );
    int count = 0;
    // Done with getHandlers to test that method.
    std::cout << "handlers:" << std::endl;
    auto vec = m_logger.getHandlers();
    for (auto handler : vec) {
        count++;
        std::cout << "    " << handler->print() << std::endl;
    }
    assert( count == 1 );

    m_logger.info() << "info using the iostream interface" << std::endl;
    m_logger.info("info using printf style interface");
    char *msg = (char *)"A random null-terminated string";
    m_logger.info("info with a char* = %s", msg);

    std::string mystring("std::string");
    const char* myother = "C String";
    char mycarr[] = "C array string";
    int aninteger = 10;
    m_logger.info() << mystring << " " << aninteger << std::endl;
    m_logger.info("mystring is %s, aninteger is %d", mystring, aninteger);
    m_logger.info("myother is %s, mycarr is %s", myother, mycarr);
    return 0;
}
#pragma GCC diagnostic pop

// Test file handler.
int LoggerTest::testFileHandler(void) {
    m_logger.clearHandlers();

    system("rm -f /tmp/*.log");

    m_logger.addHandler<MLoggerFileHandler>("/tmp/logfile.log",
        500 * 1024 * 1024, 300, true);

    // Should be the only handler.
    int count = 0;
    std::cout << "handlers:" << std::endl;
    for (auto handler : m_logger.getHandlers()) {
        count++;
        std::cout << "    " << handler->print() << std::endl;
    }
    assert( count == 1 );

    std::string badstuff("bad shit happened");

    m_logger.info() << "This is a log entry" << std::endl;
    m_logger.warning() << "error: " << badstuff << std::endl;

    m_logger.clearHandlers();

    // Are the lines in the file?
    FILE *logfile = fopen("/tmp/logfile.log", "r");
    assert( logfile != NULL );

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
    assert( count == 2 );

    system("rm -f /tmp/*.log");

    // Try again with the printf style logging
    m_logger.addHandler<MLoggerFileHandler>("/tmp/logfile.log",
        500 * 1024 * 1024, 300, true);

    m_logger.info("this is a log entry");
    m_logger.warning("error: %s", badstuff);

    m_logger.clearHandlers();

    logfile = fopen("/tmp/logfile.log", "r");
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
    assert( count == 2 );
    return 0;
}

int
main(void) {
    LoggerTest ltest;

    int rv = 0;

    ltest.setUp();
    rv = ltest.testSimple();
    ltest.tearDown();

    if (rv != 0) {
        return rv;
    }

    ltest.setUp();
    rv = ltest.testFileHandler();
    ltest.tearDown();

    return rv;
}