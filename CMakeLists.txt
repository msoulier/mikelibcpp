# 1: Set the minimum required version of CMake to be 3.10
cmake_minimum_required(VERSION 3.10)

project(mikelibcpp
    VERSION 1.0.5
    DESCRIPTION "Mike's personal C++ library"
    HOMEPAGE_URL https://github.com/msoulier/mikelibcpp
    LANGUAGES CXX)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

configure_file(config.hpp.in config.hpp)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../mikelibc dist)

add_library(mikecpp STATIC mcrypto.cpp mlogger.cpp mnetwork.cpp mstring.cpp)
    
target_include_directories(mikecpp
    PUBLIC ${PROJECT_BINARY_DIR}
    PUBLIC ${PROJECT_SOURCE_DIR}/../mikelibc)

target_link_libraries(mikecpp PUBLIC mike)

set(CMAKE_CXX_COMPILER g++)
