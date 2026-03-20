//
// Created by backo on 3/11/25.
//

#ifndef MEDES_REQUEST_PROTOCOL_H
#define MEDES_REQUEST_PROTOCOL_H

#include <functional>
#include <map>
#include <string>
#include <sstream>

#include "internal/string_convert.h"
#include "http_methods.h"
#include "internal/string_literal.h"

namespace medes {
    struct partial_request_data {
        std::multimap<std::string, std::string> headers{};
        std::string body;
        std::string mimetype = "text/plain";
    };

    struct request_data {
        http_method method = http_method::get;
        std::string url;
        std::multimap<std::string, std::string> headers;
        std::string body;
        std::string mimetype = "text/plain";


        request_data(http_method method, std::string&& url,
                     partial_request_data&& partial_data) : method(method),
                                                            url(std::move(url)),
                                                            headers(std::move(partial_data.headers)),
                                                            body(std::move(partial_data.body)),
                                                            mimetype(std::move(partial_data.mimetype)) {
        }

        request_data(http_method method, const std::string& url,
                     const partial_request_data& partial_data) : method(method), url(url),
                                                                 headers(partial_data.headers), body(partial_data.body),
                                                                 mimetype(partial_data.mimetype) {
        }
    };

    template<typename T>
    concept RequestDataConcept = std::is_base_of_v<request_data, T>;

    template<typename T>
    struct response_data_traits;

    template<typename T>
    concept ResponseDataConcept = requires
    {
        requires internal::ConvertibleToString<typename response_data_traits<T>::body_type>;
        requires std::integral<typename response_data_traits<T>::status_type>;
    };

    namespace detail {
        template<typename T>
        struct request_protocol_return_type {
            static constexpr bool is_valid = false;
            using type = void;
        };

        template<ResponseDataConcept T>
        struct request_protocol_return_type<T> {
            static constexpr bool is_valid = true;
            using type = T;
        };

        template<template<typename...> typename TemplateT, ResponseDataConcept TResponseData, typename... Ts>
        struct request_protocol_return_type<TemplateT<TResponseData, Ts...>> {
            static constexpr bool is_valid = true;
            using type = TResponseData;
        };
    }

    /**
     * @brief Either a ResponseData type or a template whose first template parameter is a ResponseData type.
     */
    template<typename T>
    concept RequestProtocolReturnType = detail::request_protocol_return_type<T>::is_valid;

    /**
     * @brief Compares a RequestProtocolReturnType type and a ResponseData type. Used to check that a function returns
     * a ResponseData type either itself or wrapped by a template.
     */
    template<typename T, typename TResponseData>
    concept SameResponseDataAs = ResponseDataConcept<TResponseData> && std::same_as<typename
                                     detail::request_protocol_return_type<T>::type, TResponseData>;

    template<typename T>
    concept HasRequestWithoutArg = requires(T t, typename T::request_data_type request_data_obj)
    {
        {
            t.request(request_data_obj)
        } -> SameResponseDataAs<typename T::response_data_type>;
    } && (std::same_as<typename T::request_arg, void> || !requires { typename T::request_arg; });


    template<typename T>
    concept HasRequestWithArg = !std::same_as<typename T::request_arg, void> /*&& requires(
                            T t, typename T::request_data_type request_data_obj,
                            typename T::request_arg arg)
                                {
                                    {
                                        t.request(request_data_obj, arg)
                                    } -> SameResponseDataAs<typename T::response_data_type>;
                                }*/;


    template<typename T>
    concept RequestProtocol = std::same_as<typename T::request_data_type, request_data> && (
                                  HasRequestWithArg<T> || HasRequestWithoutArg<T>);
}

#endif //MEDES_REQUEST_PROTOCOL_H
