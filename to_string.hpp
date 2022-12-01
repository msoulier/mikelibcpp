#ifndef TO_STRING_HPP
#define TO_STRING_HPP

/*
 * This header contains templates to map different C++ types to strings
 * for printing. This is all done via compile-time templates, transparently
 * through the MLogger class.
 */

#include <string>

#include "type_traits.hpp"

// Convert std::strings to char* for standard sprintf handling.
template<typename T>
std::enable_if_t<std::is_same_v<T, std::string>, const char *> to_string(const T& data)
{
    return data.data();
}

// Anything else, return untouched.
template<typename T>
std::enable_if_t<!std::is_same_v<T, std::string>, T> to_string(const T& data)
{
    return data;
}

#endif
