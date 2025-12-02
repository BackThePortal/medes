//
// Created by backo on 23/11/25.
//

#ifndef MEDES_STATIC_STRING_MAP_H
#define MEDES_STATIC_STRING_MAP_H

#include <string>

#include "string_convert.h"
#include "string_literal.h"

namespace medes::internal {
    template<string_literal... Keys>
    class static_string_map;

    template<bool Readonly = false>
    class static_string_map_pair_pointer {
    private:
        std::conditional_t<Readonly, const std::string&, std::string&> value;
        bool& set;

        template<string_literal... Keys>
        friend
        class static_string_map;

        static_string_map_pair_pointer(std::string_view key, const std::string& value, bool& set) requires(Readonly)
                : key(key), value(value),
                  set(set) {}


        static_string_map_pair_pointer(std::string_view key, std::string& value, bool& set) requires (!Readonly) : key(
                key), value(value),
                                                                                                                   set(set) {}

    public:
        const std::string_view key;

        [[nodiscard]] bool is_set() const {
            return this->set;
        }

        [[nodiscard]] const std::string& get_value() const {
            return this->value;
        }

        void set_value(const std::string& new_value) requires (!Readonly) {
            this->set = true;
            this->value = new_value;
        }

        void remove_value() requires (!Readonly) {
            this->set = false;
        }


    };

    /**
     * A static map of strings with compile time keys and runtime values, as well as a checker for set and unset values.
     * @tparam Keys
     */
    template<string_literal... Keys>
    class static_string_map {
    public:
        static constexpr std::size_t Size = sizeof...(Keys);
        static constexpr std::array names{std::string_view(Keys)...};

    private:
        std::array<std::string, Size> values{};
        std::array<bool, Size> set_values{false};

        /**
         * Recursive function to find the index of a key in the array.
         * @tparam Key
         * @tparam I Initial index.
         */
        template<string_literal Key, std::size_t I>

        static consteval std::size_t index_impl() {
            if constexpr (names[I] == std::string_view(Key))
                return I;
            else
                // Fully qualified call to ensure that clang-tidy formats the code correctly
                return
                        // ReSharper disable once CppRedundantTemplateKeyword
                        // ReSharper disable once CppRedundantTemplateArguments
                        static_string_map<Keys...>::template index_impl<Key, I + 1>();
        }

    public:
        static_string_map() = default;

        template<typename ... T>
        requires ((ConvertibleToString<T> && ...) && sizeof...(T) == Size)
        explicit static_string_map(T&& ... values) {
            std::size_t i = 0;
            ((this->values[i] = string_convert<T>::make_string(values), this->set_values[i] = true, ++i), ...);
        }

        template<string_literal Key> requires ((Key == Keys) || ...)

        static consteval std::size_t index() {
            return index_impl<Key, 0>();
        }


        template<string_literal Key>
        std::string& get() {
            constexpr std::size_t idx = static_string_map<Keys...>::template index<Key>();
            return this->values[idx];
        }

        template<string_literal Key>
        [[nodiscard]] const std::string& get() const {
            constexpr std::size_t idx = static_string_map<Keys...>::template index<Key>();
            return this->values[idx];
        }

        template<string_literal Key>
        [[nodiscard]] static_string_map_pair_pointer<> find() {
            constexpr std::size_t idx = static_string_map<Keys...>::template index<Key>();
            return {Key, this->values[idx], this->set_values[idx]};
        }

        template<string_literal Key>
        [[nodiscard]] static_string_map_pair_pointer<true> find() const {
            constexpr std::size_t idx = static_string_map<Keys...>::template index<Key>();
            return {Key, this->values[idx], this->set_values[idx]};
        }


        template<string_literal Key>
        std::string& set(const std::string& new_value) {
            constexpr std::size_t idx = static_string_map<Keys...>::template index<Key>();
            this->set_values[idx] = true;
            return this->values[idx] = new_value;
        }

        [[nodiscard]] bool all_set() const {
            return std::ranges::all_of(this->set_values, [](bool value_set) { return value_set; });
        }


    };

}
#endif //MEDES_STATIC_STRING_MAP_H
