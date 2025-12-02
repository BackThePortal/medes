//
// Created by backo on 17/11/25.
//

#ifndef MEDES_PATH_H
#define MEDES_PATH_H

#include <type_traits>
#include <vector>
#include "internal/string_literal.h"

namespace medes {

    /**
     * @brief Joins two paths together removing any trailing or leading slashes.
     * @tparam Left May end in a slash.
     * @tparam Right May begin with a slash.
     * @return Returns the path consisting of Left and Right joined by a single slash.
     */
    template<internal::string_literal Left, internal::string_literal Right>
    struct join_paths {
    public:
        static constexpr bool left_ends_with_slash = *(Left.end() - 2) == '/';
        static constexpr bool right_starts_with_slash = *Right.begin() == '/';
    private:

        static constexpr auto left_clean = std::conditional_t<left_ends_with_slash,
                std::integral_constant<internal::string_literal<Left.Length - 1>,
                        internal::string_literal<Left.Length - 1>::from_pointer(Left.begin())>,
                std::integral_constant<internal::string_literal<Left.Length>, Left>
        >::value;
        static constexpr auto right_clean = std::conditional_t<right_starts_with_slash,
                std::integral_constant<internal::string_literal<Right.Length - 1>,
                        internal::string_literal<Right.Length - 1>::from_pointer(
                                Right.begin() + 1)>,
                std::integral_constant<internal::string_literal<Right.Length>, Right>
        >::value;

    public:
        static constexpr auto value = left_clean + internal::string_literal("/") + right_clean;
    };

    // TODO Extract string_literal modifications to a function to avoid code duplication.
    template<internal::string_literal Right>
    struct join_paths<"", Right> {
    public:
        static constexpr bool right_starts_with_slash = *Right.begin() == '/';
    private:
        static constexpr auto right_clean = std::conditional_t<right_starts_with_slash,
                std::integral_constant<internal::string_literal<Right.Length - 1>,
                        internal::string_literal<Right.Length - 1>::from_pointer(
                                Right.begin() + 1)>,
                std::integral_constant<internal::string_literal<Right.Length>, Right>
        >::value;
    public:
        static constexpr auto value = internal::string_literal("/") + right_clean;
    };


    template<internal::string_literal Left>
    struct join_paths<Left, ""> {
    public:
        static constexpr bool left_ends_with_slash = *(Left.end() - 2) == '/';

    private:
        static constexpr auto left_clean = std::conditional_t<left_ends_with_slash,
                std::integral_constant<internal::string_literal<Left.Length - 1>,
                        internal::string_literal<Left.Length - 1>::from_pointer(Left.begin())>,
                std::integral_constant<internal::string_literal<Left.Length>, Left>
        >::value;
    public:
        static constexpr auto value = left_clean + internal::string_literal("/");
    };

/*
    template<std::size_t LengthA, std::size_t LengthB>
    constexpr auto _join_paths(internal::string_literal<LengthA> a, internal::string_literal<LengthB> b) {
        auto right_clean = (b.size() > 0 && b.back() == '/') ? std::string_view(
                internal::string_literal<b.Length - 1>::from_pointer(b.begin())) : std::string_view(b);
        auto left_clean = (a.size() > 0 && a.front() == '/') ? std::string_view(
                internal::string_literal<a.Length - 1>::from_pointer(a.begin() + 1)) : std::string_view(a);

        return internal::string_literal<left_clean.size() + 1>(left_clean) + internal::string_literal<right_clean.size() + 1>(right_clean);
    }

    static_assert(internal::string_literal("hol/adeud") ==
                  _join_paths(internal::string_literal("hol"), internal::string_literal("/adeud")));
*/
}
#endif //MEDES_PATH_H
