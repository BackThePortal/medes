//
// Created by backo on 3/11/25.
//

#ifndef MEDES_REQUEST_PROTOCOL_H
#define MEDES_REQUEST_PROTOCOL_H

#include <functional>
#include <map>
#include <string>
#include <sstream>

#include "http_methods.h"

namespace medes {
    struct partial_request_data {
        std::multimap<std::string, std::string> headers{};
        std::string body;
        std::string mimetype = "text/plain";
    };

    struct request_data {
        http_method method = http_method::get;
        std::string url;
        std::string body;
        std::string mimetype = "text/plain";
        std::multimap<std::string, std::string> headers;


        request_data(http_method method, std::string&& url,
                     partial_request_data&& partial_data) : method(method),
                                                            url(std::move(url)), body(std::move(partial_data.body)),
                                                            mimetype(std::move(partial_data.mimetype)),
                                                            headers(std::move(partial_data.headers)) {
        }
    };

    template<typename T>
    concept RequestDataConcept = std::is_base_of_v<request_data, T>;

    struct raw_response_data {
        std::stringstream body;
        int status_code = 200;
    };

    class request_protocol_base {
    public:
        virtual ~request_protocol_base() = default;
    };

    template<RequestDataConcept TRequestData, typename ReturnType, typename RequestArg = void>
    class request_protocol : request_protocol_base {
    public:
        using request_data_type = TRequestData;
        using return_type = ReturnType;
        using request_arg = RequestArg;

        virtual ReturnType request(const request_data_type& request_data_obj, RequestArg arg) = 0;
    };

    template<RequestDataConcept TRequestData, typename ReturnType>
    class request_protocol<TRequestData, ReturnType, void> : request_protocol_base {
    public:
        using request_data_type = TRequestData;
        using return_type = ReturnType;
        using request_arg = void;
        virtual ReturnType request(const request_data_type& request_data_obj) = 0;

    };

    template<typename ReturnType, RequestDataConcept TRequestData = request_data>
    using request_protocol_async = request_protocol<TRequestData, ReturnType>;


    template<typename CallbackType, RequestDataConcept TRequestData = request_data>
    using request_protocol_callback = request_protocol<TRequestData, void, CallbackType>;

    template<typename T>
    concept RequestProtocolConcept = std::is_base_of_v<request_protocol_base, T>;


    class dummy_request_protocol final : public request_protocol_callback<std::function<void(raw_response_data)>> {
    public:
        void request(const request_data_type& request_data_obj,
                     const std::function<void(raw_response_data)> func) override {
            std::stringstream ss;
            ss << "This is a dummy response.";
            func(raw_response_data{(std::move(ss)), 200});
        };
    };
}

#endif //MEDES_REQUEST_PROTOCOL_H
