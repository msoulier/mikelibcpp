#include <cppunit/ui/text/TestRunner.h>

#include "safequeuetest.hpp"
#include "loggertest.hpp"
#include "cryptotest.hpp"

int main( int argc, char **argv)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( SafeQueueTest::suite() );
    runner.addTest( LoggerTest::suite() );
    runner.addTest( CryptoTest::suite() );
    return !runner.run();
}
