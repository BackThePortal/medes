//
// Created by backo on 3/11/25.
//

#ifndef MEDES_QUERY_PARAMS_H
#define MEDES_QUERY_PARAMS_H

#include "medes/internal/string_literal.h"
#include "medes/internal/meta.h"
#include "medes/internal/string_convert.h"
#include "string_build.h"
#include <nlohmann/json.hpp>

namespace medes {
    enum class query_key_requirement {
        REQUIRED, OPTIONAL
    };

    /**
     * @brief The query_value_convert enum is used to specify how to convert a query value to a string. You do not need
     * to specify this if the value is a string or a bool.
     */
    enum class query_value_convert {
        /**
         * @brief std::string values are left as is, bool values are converted to "true" or "false", and other types are
         * converted to strings using internal::string_convert.
         */
        DEFAULT,
        /**
         * @brief Convert the value to a nlohmann::json object and then use the json::get\<std::string> endpoint to convert
         * it to a string.
         */
        THROUGH_JSON,
    };

    /*
     * The derived classes optional and required are used for specifying which query parameters a endpoint accepts.
     * To pass query values to methods, use query_value or better, the query function.
     *
     * TODO Default value.
     */
    template<internal::string_literal TKey, typename ValueType = std::string,
        query_value_convert ConvertPolicy = query_value_convert::DEFAULT>
    struct query_base {
        struct [[maybe_unused]] _query_base_tag {
        };

        static_assert(internal::ConvertibleToString<ValueType> || std::is_same_v<ValueType, bool> ||
                      ConvertPolicy == query_value_convert::THROUGH_JSON);

        static constexpr internal::string_literal Key = TKey;

        explicit query_base(bool b) requires std::is_same_v<ValueType, bool>
            : value(b ? "true" : "false") {
        }


        explicit query_base(ValueType value) requires(!std::is_same_v<ValueType, bool> &&
                                                      internal::ConvertibleToString<ValueType>)
            : value(internal::string_convert<ValueType>(value)) {
        }


        explicit query_base(ValueType value) requires(ConvertPolicy == query_value_convert::THROUGH_JSON)
            : value(nlohmann::json(value).get<std::string>()) {
        }

        query_base() = default;

        std::string value;
    };

    template<internal::string_literal Key, typename ValueType = std::string,
        query_value_convert ConvertPolicy = query_value_convert::DEFAULT>
    struct required : query_base<Key, ValueType, ConvertPolicy> {
        struct [[maybe_unused]] _query_descr_tag {
        };

        using query_base<Key, ValueType, ConvertPolicy>::query_base;

        static constexpr query_key_requirement get_requirement() { return query_key_requirement::REQUIRED; }
    };

    template<internal::string_literal Key, typename ValueType = std::string,
        query_value_convert ConvertPolicy = query_value_convert::DEFAULT>
    struct optional : query_base<Key, ValueType, ConvertPolicy> {
        struct [[maybe_unused]] _query_descr_tag {
        };

        using query_base<Key, ValueType, ConvertPolicy>::query_base;

        static constexpr query_key_requirement get_requirement() { return query_key_requirement::OPTIONAL; }
    };

    /**
     * Check if T is a template instantiation of an optional or required query key.
     */
    template<typename T>
    concept QueryKeyDescr = requires { typename T::_query_descr_tag; };


    template<typename T>
    concept QueryBase = requires { typename T::_query_base_tag; };


    template<internal::string_literal TKey, typename ValueType>
    struct query_value {
        struct [[maybe_unused]] _query_value_tag {
        };

        static constexpr internal::string_literal Key = TKey;


        ValueType value;

        /**
         * @brief Convert a query into a query_base for use in endpoint. This indirection allows
         * checking that a possible conversion to string exists while only specifying the
         * Key template parameter
         */
        template<QueryKeyDescr QueryTarget>
        QueryTarget get_query() {
            return QueryTarget{value};
        }
    };

    // TODO Can this be done with a deduction guide instead?
    /**
     * @brief Create a query_value object from a value, deducing the type of the query value.
     */
    template<internal::string_literal TKey>
    constexpr auto query(auto value) {
        return query_value<TKey, decltype(value)>(value);
    }

    template<typename T>
    concept QueryValue = requires { typename T::_query_value_tag; };



    struct validate_query_parameters_result {
        std::optional<std::string_view> duplicate = std::nullopt;
        int missing_required = 0;
        std::optional<std::string_view> unknown_key = std::nullopt;
    };


    /**
     *
     * @tparam QueryKeys The query keys description of the endpoint.
     */
    template<QueryKeyDescr... QueryKeys>
    struct query_params_handler {
    private:
        query_params_handler() = default;

        static consteval bool check() {
            constexpr std::array<std::string_view, sizeof...(QueryKeys)> all_query_keys = {
                std::string_view(QueryKeys::Key)...
            };

            // Check for duplicates
            for (std::size_t i = 0; i < all_query_keys.size(); ++i) {
                for (std::size_t j = i + 1; j < all_query_keys.size(); ++j) {
                    if (all_query_keys[i] == all_query_keys[j]) return false;
                }
            }

            return true;
        }

        static_assert(check());

        template<internal::string_literal Key, QueryKeyDescr First, QueryKeyDescr... Rest>
        static constexpr std::size_t find_query_by_key_impl(int n = 0) {
            if constexpr (First::Key == Key) return n;
            else {
                static_assert(sizeof...(Rest) == 0, "Query by key not found");
                return find_query_by_key_impl<Key, Rest...>(n + 1);
            }
        }

    public:
        static constexpr std::size_t num = sizeof...(QueryKeys);

        /**
         *
         * @tparam TargetQueryKeys The query values that have to be validated.
         * @return
         */
        template<QueryValue... TargetQueryValues>
        static consteval validate_query_parameters_result validate() {
            constexpr std::array<std::pair<std::string_view, query_key_requirement>, sizeof...(QueryKeys)> allQueryKeys
                    = {
                        std::pair{std::string_view(QueryKeys::Key), QueryKeys::get_requirement()}...
                    };
            constexpr std::array<std::string_view, sizeof...(TargetQueryValues)> targetKeys = {
                std::string_view(TargetQueryValues::Key)...
            };


            // Check for duplicates
            for (std::size_t i = 0; i < targetKeys.size(); ++i) {
                for (std::size_t j = i + 1; j < targetKeys.size(); ++j) {
                    if (targetKeys[i] == targetKeys[j]) return {.duplicate = targetKeys[i]}; // duplicate
                }
            }

            // Count required keys
            int requiredKeysNum = 0;
            for (const auto& [key, key_type] : allQueryKeys) {
                if (key_type == query_key_requirement::REQUIRED) { ++requiredKeysNum; }
            }

            // Check for unknown keys and missing required keys
            // TODO Use a set instead of an array.
            for (const auto& key : targetKeys) {
                bool found = false;
                for (auto& [k, type] : allQueryKeys) {
                    if (key == k) {
                        found = true;
                        if (type == query_key_requirement::REQUIRED) --requiredKeysNum;
                        break;
                    }
                }
                if (!found) return {.unknown_key = key};
            }

            return {.missing_required = requiredKeysNum};
        }


        template<std::size_t Index>
        using find_query_by_index = internal::get_type_from_pack<Index, QueryKeys...>;

        template<internal::string_literal Key>
        using find_query_by_key = find_query_by_index<find_query_by_key_impl<Key, QueryKeys...>()>;

        static constexpr std::string get_query_value(QueryValue auto query_key) {
            using candidate_query = find_query_by_key<decltype(query_key)::Key>;

            return query_key.template get_query<candidate_query>().value;
        }

        /**
         * @brief Returns a map in which the keys are the query keys and the values are the query values.
         * @example get_query_map(query<"key1">("value1"), query<"key2">("value2"))
         */
        template<QueryValue FirstQueryKey, QueryValue... Rest>
        static constexpr std::map<std::string, std::string>
        get_query_map(FirstQueryKey first_query_key, Rest... rest_queries) {
            std::map<std::string, std::string> map;

            if constexpr (sizeof...(rest_queries) != 0) map = get_query_map(rest_queries...);

            map[FirstQueryKey::Key.data] = get_query_value(first_query_key);

            return map;
        }


        /**
         * @brief Validate and create a query string from a list of query values created with the query function.
         */
        template<QueryValue... TargetQueryValues>
        static std::string validate_and_make(TargetQueryValues... query_raw) {
            constexpr auto validate_query_result = validate<TargetQueryValues...>();
            static_assert(validate_query_result.unknown_key == std::nullopt, "Found unknown query key ");
            static_assert(validate_query_result.duplicate == std::nullopt, "Found duplicate query key");
            static_assert(validate_query_result.missing_required == 0, "Missing required query keys");

            if constexpr (sizeof...(query_raw) != 0) {
                std::map<std::string, std::string> query_params = get_query_map<TargetQueryValues...>(query_raw...);
                return make_query(query_params);
            } else {
                return {""};
            }
        }
    };
}

#endif //MEDES_QUERY_PARAMS_H
