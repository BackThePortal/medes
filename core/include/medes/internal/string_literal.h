//
// Created by backo on 3/11/25.
//

#ifndef MEDES_STRING_LITERAL_H
#define MEDES_STRING_LITERAL_H


#include <algorithm>
#include <string_view>

namespace medes::internal {
    /**
     * @tparam N Length of the char array. This includes the ending null character.
     *
     * @see https://dev.to/sgf4/strings-as-template-parameters-c20-4joh
     */
    template<std::size_t N>
    struct string_literal {
        char data[N]{};

        /**
         * Includes NULL character.
         */
        static constexpr std::size_t Length = N;

        consteval explicit string_literal(char c) {
            std::fill_n(this->data, N - 1, c);
            this->data[N - 1] = '\0';
        }

        consteval string_literal(const char (& str)[N]) {
            std::copy_n(str, N, this->data);
        }// NOLINT(*-explicit-constructor)

        consteval string_literal(std::string_view sv) {
            // N must be sv.size() + 1
            std::copy_n(sv.data(), N - 1, this->data);
            this->data[N - 1] = '\0'; // Ensure null termination
        }

        static consteval string_literal<N> from_pointer(const char* begin) {
            char str[N]{};
            std::copy_n(begin, N - 1, str);
            return {str};
        }


        consteval bool operator==(const string_literal<N> str) const {
            return std::equal(str.data, str.data + N, this->data);
        }

        template<std::size_t N2>
        consteval bool operator==(const string_literal<N2> s) const {
            return false;
        }

        template<std::size_t N2>
        consteval string_literal<N + N2 - 1> operator+(const string_literal<N2> str) const {
            char newchar[N + N2 - 1]{};
            std::copy_n(this->data, N - 1, newchar);
            std::copy_n(str.data, N2, newchar + N - 1);
            return newchar;
        }

        consteval char operator[](std::size_t n) const { return this->data[n]; }

        /**
         * Does NOT include NULL character.
         * @return
         */
        [[nodiscard]] consteval std::size_t size() const { return N - 1; }

        constexpr operator std::string_view() const {// NOLINT(*-explicit-constructor)
            return std::string_view(this->data, N - 1);
        }

        /**
         * @param c - The character to count.
         * @returns The number of times c appears in the string.
         */
        [[nodiscard]] consteval int count(char c) const {
            int n = 0;
            for (int i = 0; i < N; ++i) {
                if (this->data[i] == c) ++n;
            }
            return n;
        }

        /**
         * @returns A pointer to the first character of the string. If it's empty, begin() == end().
         */
        [[nodiscard]] constexpr char* begin() { return this->data; }
        [[nodiscard]] constexpr const char* begin() const { return this->data; }
        [[nodiscard]] constexpr const char* cbegin() const  { return this->data; }

        /**
         * @returns A pointer past the last character of the string. If it's empty, begin() == end().
         */
        [[nodiscard]] constexpr char* end() { return this->data + N; }
        [[nodiscard]] constexpr const char* end() const { return this->data + N; }
        [[nodiscard]] constexpr const char* cend() const { return this->data + N; }

        [[nodiscard]] constexpr char& back() { return *(this->end() - 2); }

        [[nodiscard]] constexpr const char& back() const { return *(this->end() - 2); }

        [[nodiscard]] constexpr char& front() { return *this->data; }

        [[nodiscard]] constexpr const char& front() const { return *this->data; }
    };


    template<std::size_t s1, std::size_t s2>
    consteval auto operator+(string_literal<s1> fs, const char (& str)[s2]) {
        return fs + string_literal<s2>(str);
    }

    template<std::size_t s1, std::size_t s2>
    consteval auto operator+(const char (& str)[s2], string_literal<s1> fs) {
        return string_literal<s2>(str) + fs;
    }

    template<std::size_t s1, std::size_t s2>
    consteval auto operator==(string_literal<s1> fs, const char (& str)[s2]) {
        return fs == string_literal<s2>(str);
    }

    template<std::size_t s1, std::size_t s2>
    consteval auto operator==(const char (& str)[s2], string_literal<s1> fs) {
        return string_literal<s2>(str) == fs;
    }

    template<std::size_t s>
    consteval bool ends_with(string_literal<s> str, char c) {
        return str.back() == c;
    }

    template<std::size_t s1, std::size_t s2>
  consteval bool ends_with(string_literal<s1> str, string_literal<s2> postfix) {
        return str.size() >= postfix.size() && std::equal(postfix.cbegin(), postfix.cend(), str.cend() - postfix.size());
    }

    template<std::size_t s>
    consteval bool starts_with(string_literal<s> str, char c) {
        return str.front() == c;
    }

    template<std::size_t s1, std::size_t s2>
    consteval bool starts_with(string_literal<s1> str, string_literal<s2> prefix) {
        return str.size() >= prefix.size() && std::equal(prefix.cbegin(), prefix.cend(), str.cbegin());
    }


    template<std::size_t s1, std::size_t s2>
    consteval auto remove_postfix(string_literal<s1> str, string_literal<s2> postfix) {
        if (ends_with(str, postfix)) {
            return string_literal<s1 - postfix.size() + 1>(std::string_view(str.cbegin(), str.cend() - postfix.size()));
        }
        return str;
    }

    template<std::size_t s1, std::size_t s2>
    consteval auto remove_prefix(string_literal<s1> str, string_literal<s2> prefix) {
        if (starts_with(str, prefix)) {
            return string_literal<s1 - prefix.size() + 1>(std::string_view(str.cbegin() + prefix.size(), str.cend()));
        }
        return str;
    }
/*
   template<std::size_t N, std::size_t Index, auto Func>
    consteval auto get_literal_at() {
        return string_literal<Func()[Index].size() + 1>{Func()[Index]};
    }
*/

    constexpr char to_upper(char c) {
        return (c >= 'a' && c <= 'z') ? static_cast<char>(c - 'a' + 'A') : c;
    }

    constexpr char to_lower(char c) {
        return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
    }


    template<std::size_t N>
    constexpr std::array<char, N> to_upper(const char (& input)[N]) {
        std::array<char, N> result = {};
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = to_upper(input[i]);
        }
        return result;
    }

    template<std::size_t N>
    constexpr std::array<char, N> to_lower(const char (& input)[N]) {
        std::array<char, N> result = {};
        for (std::size_t i = 0; i < N; ++i) {
            result[i] = to_lower(input[i]);
        }
        return result;
    }
}// namespace dpp_structures::internal
#endif //MEDES_STRING_LITERAL_H
