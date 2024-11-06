#include <thread>
#include <iostream>
#include <vector>
#include <assert.h>

#include "test_mstring.hpp"

int StringTest::testBasic(void) {
    MString mystring("Hello, World!");
    MString other = mystring;

    std::cout << "mystring is " << mystring << std::endl;

    std::vector<std::string> pieces = mystring.split(",");

    for (auto &it : pieces) {
        std::cout << "piece: " << it << std::endl;
    }

    assert( pieces.size() == 2 );

    mystring = "this:is:a:delimited:string";
    pieces = mystring.split(":");

    for (auto &it : pieces) {
        std::cout << "piece: " << it << std::endl;
    }

    assert( pieces.size() == 5 );

    return 0;
}

int
main(void) {
    StringTest stest;
    return stest.testBasic();
}