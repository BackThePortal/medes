//
// Created by backo on 3/11/25.
//

#ifndef MEDES_PATH_PARAMS_H
#define MEDES_PATH_PARAMS_H

#include <type_traits>
#include <concepts>
#include <array>
#include <map>
#include <string>
#include <memory>
#include <vector>

#include "medes/internal/string_literal.h"
#include "medes/internal/string_convert.h"
#include "medes/internal/static_string_map.h"


namespace medes {
    class path_params_container {
    public:
    };

    template<int N>
    using path_params_container_params_type = std::array<std::string, N>;

    template<typename T>
    concept PathParamsContainer =
            std::is_base_of_v<path_params_container, T> &&
            requires(const T& t)
            {
                // Must accept empty pack
                { t.template validate<>() } -> std::convertible_to<bool>;

                // Must accept at least one element
                {
                    t.template validate<"a">()
                }
                -> std::convertible_to<bool>;
                /**
                 * @brief Returns a vector of N references to the parameters.
                 * @pre validate() == true && instant_validate<N>() == true
                 * @param names The N names of the parameters.
                 * @return A vector of N references to the parameters values, in the same order as the names vector.
                 */
                { t.template get_params<>() } -> std::ranges::random_access_range;
                {
                    t.template get_params<"a">()
                } -> std::ranges::random_access_range;
            } && requires
            {
                // The following doesn't check for compile-time evaluation.
                // Must accept empty pack
                { T::template instant_validate<>() } -> std::convertible_to<bool>;

                // Must accept at least one element
                {
                    T::template instant_validate<"a">()
                }
                -> std::convertible_to<bool>;
            };


    template<typename T, auto MemberFunction>
    concept PathParamsProviderWithMemberFunction = requires(T t)
    {
        { (t.*MemberFunction)() } -> PathParamsContainer;
    };
    template<typename T, auto Function>
    concept PathParamsProviderWithFunction = requires(T t)
    {
        { Function(t) } -> PathParamsContainer;
    };


    template<std::size_t N>
    class path_params_list : path_params_container {
    public:
        std::array<std::string, N> array;

        /**
         * @tparam T Any value convertible to std::string using string_convert.
         * @param values The parameters to pass to the path.
         */
        template<typename... T>
        explicit path_params_list(T&&... values) : array({
            internal::string_convert<T>::make_string(std::move(values))...
        }) {
        }

        template<internal::string_literal... Names>
        static consteval bool instant_validate() {
            return N == sizeof...(Names);
        }

        template<internal::string_literal... Names>
        [[nodiscard]] bool validate() const {
            // When the array is created with the correct size, all necessary values are set.
            return true;
        }

        template<internal::string_literal... Names>
        [[nodiscard]] path_params_container_params_type<sizeof...(Names)>
        get_params() const {
            path_params_container_params_type<sizeof...(Names)> params;

            int i = 0;
            for (const auto& param : this->array) {
                params[i] = param;
                ++i;
            }

            return params;
        }
    };

    template<typename... T>
    path_params_list(T&&...) ->
        path_params_list<sizeof...(T)>;

    template<internal::string_literal... Names>
    class path_params_static_map : path_params_container, public internal::static_string_map<Names...> {
    public:
        using internal::static_string_map<Names...>::static_string_map;

        template<internal::string_literal... TNames>
        [[nodiscard]] bool validate() const {
            return this->all_set();
        }

        template<internal::string_literal... TNames>
        static consteval bool instant_validate() {
            return (sizeof...(TNames) == sizeof...(Names)) && ((TNames == Names) && ...);
        }

        template<internal::string_literal... TNames>
        [[nodiscard]] std::vector<std::string> get_params() const {
            std::vector<std::string> params;
            params.reserve(this->Size);

            (([&]<internal::string_literal Name>() {
                params.push_back((*this).template get<Name>());
            }).template operator()<TNames>(), ...);

            return params;
        }
    };

    inline path_params_static_map<"a", "b"> a_map;


    class path_params_map : path_params_container {
    public:
        using map_type = std::map<std::string, std::string>;

    private:
        /**
         * @pre map_ptr != nullptr
         */
        std::unique_ptr<map_type> map_ptr;

    public:
        /**
         * @brief Creates a copy of a map and creates a path_params_map instance with the copy.
         * @param map_arg The map to copy.
         */
        explicit path_params_map(const map_type& map_arg) : map_ptr(std::make_unique<map_type>(map_arg)) {
        }

        /**
         * @brief Creates a path_params_map instance that takes ownership of a map. Use std::move to pass a unique_ptr
         * to the map you want to take ownership of.
         * @param map_ptr_arg A unique_ptr to the map to take ownership of.
         */
        explicit path_params_map(std::unique_ptr<map_type> map_ptr_arg)
            : map_ptr(std::move(map_ptr_arg)) {
        }

        /**
         * @brief Creates an empty map.
         */
        path_params_map() : map_ptr(std::make_unique<map_type>()) {
        }

        template<typename T>
        auto operator[](T&& value) {
            return this->map_ptr->find(internal::string_convert<T>::make_string(value));
        }


        template<internal::string_literal... Names>
        static consteval bool instant_validate() {
            return true;
        }

        template<internal::string_literal... Names>

        [[nodiscard]] bool validate() const {
            std::array<std::string_view, sizeof...(Names)> keys = {std::string_view(Names)...};

            return sizeof...(Names) == this->map_ptr->size() && std::ranges::all_of(keys, [this](std::string_view key) {
                return this->map_ptr->contains(std::string(key));
            });
        }

        template<internal::string_literal... Names>
        [[nodiscard]] path_params_container_params_type<sizeof...(Names)> get_params() const {
            path_params_container_params_type<sizeof...(Names)> params;
            // Suppose validate() == true, so sizeof...(Names) == this->map_ptr->size() (see above)

            int i = 0;
            (([&]<internal::string_literal Name>() {
                params[i++] = (this->map_ptr->find(std::string(Name))->second);
            }).template operator ()<Names>(), ...); // Postfix increment!!

            return params;
        }
    };


    struct path_params_check_result {
        /**
         * @brief The number of parameters found, that is, the number of bracket pairs. If ok is false, this is undefined.
         */
        std::size_t count;
        /**
         * @brief Whether the parameters were valid, that is, all brackets are closed.
         */
        bool ok;

        /**
         * @brief Whether all parameters have names, that is, the substring between the brackets is not empty.
         */
        bool all_have_names;

        constexpr bool operator==(const path_params_check_result&) const = default;
    };

    static_assert(PathParamsContainer<path_params_list<1>>);
    static_assert(PathParamsContainer<path_params_static_map<"a", "b">>);
    static_assert(PathParamsContainer<path_params_map>);

    template<typename>
    struct generic_path_params;

    template<typename... Ts>
    struct generic_path_params<std::tuple<Ts...>> {
    };

    template<internal::string_literal LiteralURL>
    struct path_params_handler {
    public:
        static constexpr std::string_view url = LiteralURL.data;

    private:
        path_params_handler() = default;

        static consteval path_params_check_result check() {
            // We use result.ok to check if we're NOT inside a bracket pair.
            path_params_check_result result{0, true, true};

            char prev = '\0';
            for (const char c : url) {
                if ((!result.ok && c == '{') || (result.ok && c == '}')) return {0, false};

                if (c == '{') result.ok = false;
                else if (c == '}') {
                    if (prev == '{') result.all_have_names = false;
                    result.ok = true;
                    ++result.count;
                }
                prev = c;
            }
            return result;
        }

        static_assert(check().ok, "Unclosed brackets");
        static_assert(check().all_have_names, "Unnamed parameter");


        static consteval std::size_t count() {
            constexpr auto result = check();
            return result.count;
        }

        /**
         * @pre check().ok == true
         * @return
         */
        static consteval std::array<std::string_view, count()> extract_names() {
            std::array<std::string_view, count()> extracted_names;

            int idx = 0;
            std::string_view::iterator current_name_start = url.end();

            for (std::string_view::iterator it = url.begin(); it != url.end(); ++it) {
                if (*it == '{') {
                    current_name_start = it + 1;
                } else if (*it == '}') {
                    extracted_names[idx] = std::string_view(current_name_start, it);
                    ++idx;
                }
            }

            return extracted_names;
        }

        static constexpr auto apply_names_to_function(auto func) -> decltype(auto) {
            return [&]<std::size_t... Is>(std::index_sequence<Is...>) -> decltype(auto) {
                return func.template operator()<
                    internal::string_literal<names[Is].size() + 1>{names[Is]}...
                >();
            }(std::make_index_sequence<num>{});
        }

    public:
        static constexpr std::size_t num = count();

        static constexpr std::array names = extract_names();

        template<PathParamsContainer Container> requires (apply_names_to_function(
            []<internal::string_literal... Names>() {
                return Container::template instant_validate<Names...>();
            }))
        static std::string validate_and_make(const Container& path_params) {
            // This could be a constraint, but CLion cannot read 'num' because it's a dependent constexpr variable (?), so
            // it always flags it as an error.

            if (!apply_names_to_function([&]<internal::string_literal... Names>() {
                return path_params.template validate<Names...>();
            }))
                throw std::runtime_error("Path parameters runtime validation failed");


            const std::array<std::string, num> params_values = apply_names_to_function(
                [&]<internal::string_literal... Names>() {
                    return path_params.template get_params<Names...>();
                });

            std::string new_url;
            auto path_params_it = params_values.cbegin();
            for (std::string_view::const_iterator url_it = url.cbegin(); url_it != url.cend(); ++url_it) {
                if (*url_it == '{') {
                    new_url.append(*path_params_it);
                    ++path_params_it;
                    do { ++url_it; } while (*url_it != '}');
                } else
                    new_url.push_back(*url_it);
            }

            return new_url;
        }

        template<typename T, auto MemberFunction> requires PathParamsProviderWithMemberFunction<T, MemberFunction>
        static std::string validate_and_make(const T& object) {
            return validate_and_make((object.*MemberFunction)());
        }

        template<typename T, auto Function> requires PathParamsProviderWithFunction<T, Function>
        static std::string validate_and_make(const T& object) {
            return validate_and_make(Function(object));
        }

        template<typename T> requires PathParamsProviderWithMemberFunction<T, &T::get_params>
        static std::string validate_and_make(const T& object) {
            return validate_and_make<T, &T::get_params>(object);
        }

    };
}

#endif //MEDES_PATH_PARAMS_H
