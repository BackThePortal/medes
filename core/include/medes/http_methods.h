//
// Created by backo on 3/11/25.
//

#ifndef MEDES_HTTP_METHODS_H
#define MEDES_HTTP_METHODS_H

#include <string>
#include <utility>

namespace medes {
    /**
     * For compatibility with D++, the values must be in the exact same order
     * as the enum dpp::http_method.
     */
    enum class http_method {
        get,
        post,
        put,
        patch,
        _delete // delete is a reserved keyword
    };

    /**
     * Converts an HTTP method enumerator to its corresponding string representation.
     *
     * @param method The HTTP method enum value to be converted.
     * @return A constant pointer to the string representation of the input HTTP method.
     *         If the input value does not match a recognized HTTP method, it may return
     *         a default or null value, depending on implementation.
     */
    [[nodiscard]] std::string http_method_to_str(http_method method) noexcept;
}

#endif //MEDES_HTTP_METHODS_H
