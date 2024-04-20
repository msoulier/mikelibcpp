/*

 * Defining is_string type handlers for the different types in C++.
 * Currently supporting C++17 or higher.
 * Copyright Jaan Soulier 2022.
 */
#ifndef type_traits_hpp
#define type_traits_hpp

#include <utility>
#include <tuple>
#include <type_traits>
#include <string>
#include <string_view>

template<typename Type>
struct is_arithmetic : std::is_arithmetic<Type> {};

template<typename Type>
struct is_enum : std::is_enum<Type> {};

template<typename Type>
struct is_string : std::false_type {};

template<typename Char, typename Traits, typename Allocator>
struct is_string<std::basic_string<Char, Traits, Allocator>> : std::true_type {};

template<typename Type>
struct is_string_view : std::false_type {};

template<typename Char, typename Traits>
struct is_string_view<std::basic_string_view<Char, Traits>> : std::true_type {};

template<typename Type>
struct is_tuple : std::false_type {};

template<typename ...Args>
struct is_tuple<std::tuple<Args...>> : std::true_type {};

template<typename Type>
struct is_pair : std::false_type {};

template<typename Left, typename Right>
struct is_pair<std::pair<Left, Right>> : std::true_type {};

template<typename Type, typename Enable = void>
struct is_container : std::false_type {};

template<typename Char, typename Traits, typename Allocator>
struct is_container<std::basic_string<Char, Traits, Allocator>> : std::false_type {};

template<typename Char, typename Traits>
struct is_container<std::basic_string_view<Char, Traits>> : std::false_type {};

template<typename Type>
struct is_container<Type, std::void_t<decltype(std::declval<Type>().begin())>> : std::true_type {};

#endif
