//
// Created by backo on 26/12/25.
//

#ifndef MEDES_CORE_ERRORS_H
#define MEDES_CORE_ERRORS_H

#include <version>

#if defined(__cpp_static_assert) && __cpp_static_assert >= 202306L
    // C++26 version: Dynamic string message
    #define MEDES_VALIDATE_PARAMS(cond, msg_expr) \
    static_assert(cond, msg_expr)
#else
    // C++20/23 version: Fallback to template trace and a static string
    #define MEDES_VALIDATE_PARAMS(cond, msg_expr) \
    static_assert(cond, "Medes static error: See template instantiation trace for details.");
#endif

#endif //MEDES_CORE_ERRORS_H