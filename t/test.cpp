#include <cppunit/ui/text/TestRunner.h>

#include "safequeuetest.hpp"
#include "loggertest.hpp"
#include "cryptotest.hpp"
#include "mstringtest.hpp"

int main( int argc, char **argv)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( SafeQueueTest::suite() );
    runner.addTest( LoggerTest::suite() );
    runner.addTest( CryptoTest::suite() );
    runner.addTest( StringTest::suite() );
    return !runner.run();
}
