//
// Created by backo on 24/12/25.
//

#ifndef MEDES_CORE_URL_H
#define MEDES_CORE_URL_H

#include <string>
#include "medes/internal/string_convert.h"
#include "medes/internal/string_literal.h"

namespace medes {
    namespace detail {
        template<internal::string_literal StrA, internal::string_literal StrB>
        struct join_urls_impl {
        private:
            static constexpr auto str_a_clean = internal::remove_postfix(StrA, '/');
            static constexpr auto str_b_clean = internal::remove_postfix(StrB, '/');

        public:
            static constexpr auto value = str_a_clean + "/" + str_b_clean;
        };
    }


    template<internal::string_literal First, internal::string_literal... Urls>
    constexpr auto join_urls() {
        if constexpr (sizeof...(Urls) == 0) return First;
        if constexpr (sizeof...(Urls) == 1) {
            return internal::remove_postfix(First, internal::string_literal("/")) + "/" + (internal::remove_prefix(
                       Urls, internal::string_literal("/")) + ...);
        } else return join_urls<First, join_urls<Urls...>()>();
    }
}

#endif //MEDES_CORE_URL_H
