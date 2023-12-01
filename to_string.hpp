#ifndef TO_STRING_HPP
#define TO_STRING_HPP

/*
 * This header contains templates to map different C++ types to strings
 * for printing. This is all done via compile-time templates, transparently
 * through the MLogger class.
 */

#include <string>

#include "type_traits.hpp"

template<typename T>
auto to_string(const T &data)
{
    if constexpr (is_string<T>::value) {
        return data.c_str();
    } else {
        return data;
    }
}

#endif
