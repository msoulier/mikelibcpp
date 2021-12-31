#include <cppunit/ui/text/TestRunner.h>

#include "safequeuetest.hpp"

int main( int argc, char **argv)
{
    CppUnit::TextUi::TestRunner runner;
    runner.addTest( SafeQueueTest::suite() );
    //runner.addTest( ComplexNumberTest::suite() );
    runner.run();
    return 0;
}
