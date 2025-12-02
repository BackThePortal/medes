//
// Created by backo on 12/11/25.
//

#ifndef MEDES_STRING_CONVERT_H
#define MEDES_STRING_CONVERT_H

#include <concepts>
#include <type_traits>
#include <string>
#include "meta.h"

namespace medes::internal {
    template<typename>
    struct string_convert;

    template<typename T> requires requires(T t) {
        std::to_string(t);
    }
    struct string_convert<T> {
        static std::string make_string(T&& number) {
            return std::to_string(number);
        }
    };

    template<typename T> requires explicitly_convertible_to<T, std::string>
    struct string_convert<T> {
        static std::string make_string(T&& s) {
            return std::string(s);
        }
    };

    /*
     * There shouldn't be any conflict between these last two specializations
     * because std::to_string is for builtin numerical types, for which the
     * std::string constructor isn't defined.
     */

/**
     * @brief A type that can be converted to a string using string_convert.
     */
    template<typename T>
    concept ConvertibleToString = requires {
        string_convert<T>::make_string;
    };

};

#endif //MEDES_STRING_CONVERT_H
