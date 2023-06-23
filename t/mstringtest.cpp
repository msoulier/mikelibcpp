#include <thread>
#include <iostream>
#include <vector>

#include "mstringtest.hpp"

void StringTest::testBasic(void) {
    MString mystring("Hello, World!");
    MString other = mystring;

    std::cout << "mystring is " << mystring.str() << std::endl;

    std::vector<std::string> pieces = mystring.split(",");

    for (auto &it : pieces) {
        std::cout << "piece: " << it << std::endl;
    }

    CPPUNIT_ASSERT( pieces.size() == 2 );

    mystring = "this:is:a:delimited:string";
    pieces = mystring.split(":");

    for (auto &it : pieces) {
        std::cout << "piece: " << it << std::endl;
    }

    CPPUNIT_ASSERT( pieces.size() == 5 );
}
