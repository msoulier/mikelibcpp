#include <cppunit/ui/text/TestRunner.h>

#include "safequeuetest.hpp"
#include "loggertest.hpp"

int main( int argc, char **argv)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( SafeQueueTest::suite() );
    runner.addTest( LoggerTest::suite() );
    return !runner.run();
}
