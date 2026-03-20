//
// Created by backo on 11/3/26.
//

#ifndef MEDES_DPP_ADAPTER_H
#define MEDES_DPP_ADAPTER_H

#include <dpp/dpp.h>

#include "medes/request_protocol.h"

template<>
struct medes::response_data_traits<dpp::http_request_completion_t> {
    using body_type = decltype(dpp::http_request_completion_t::body);
    using status_type = decltype(dpp::http_request_completion_t::status);
};

namespace medes::dpp_adapter {
    /**
     * @pre bot != nullptr
     */
    inline dpp::async<dpp::http_request_completion_t> coro_request(
        dpp::cluster* bot, const request_data& request_data) {
        // I'll assume that postdata is just the request body...
        return bot->co_request(request_data.url, static_cast<dpp::http_method>(request_data.method), request_data.body,
                               request_data.mimetype, request_data.headers);
    }

    inline void request(dpp::cluster* bot, const request_data& request_data, const dpp::http_completion_event& callback) {
        bot->request(request_data.url, static_cast<dpp::http_method>(request_data.method), callback, request_data.body,
                               request_data.mimetype, request_data.headers);
    }

    class basic_dpp_request_protocol {
    protected:
        dpp::cluster* cluster_ = nullptr;
    public:
        using request_data_type = request_data;
        using response_data_type = dpp::http_request_completion_t;
        basic_dpp_request_protocol() = default;
        void attach(dpp::cluster* cluster) {
            this->cluster_ = cluster;
        }

        [[nodiscard]] bool is_attached() const noexcept {
            return this->cluster_ != nullptr;
        }

        void throw_if_not_attached() const {
            if (!this->is_attached()) {
                throw std::runtime_error("dpp_coro_request_protocol is not attached to a cluster");
            }
        }
    };

    class dpp_coro_request_protocol : public basic_dpp_request_protocol {
    public:
        // ReSharper disable once CppMemberFunctionMayBeConst
        dpp::async<dpp::http_request_completion_t> request(const request_data_type& request_data_obj) {
            this->throw_if_not_attached();

            return coro_request(this->cluster_, request_data_obj);
        };
    };

    class dpp_request_protocol : public basic_dpp_request_protocol {
    public:
        using request_arg = const dpp::http_completion_event&;
        // ReSharper disable once CppMemberFunctionMayBeConst
        void request(const request_data_type& request_data_obj, const dpp::http_completion_event& callback) {
            this->throw_if_not_attached();

            dpp_adapter::request(this->cluster_, request_data_obj, callback);
        }
    };

    static_assert(RequestProtocol<dpp_coro_request_protocol>);
    static_assert(RequestProtocol<dpp_request_protocol>);
}

#endif //MEDES_DPP_ADAPTER_H
