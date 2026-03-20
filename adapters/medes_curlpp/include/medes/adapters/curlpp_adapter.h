//
// Created by backo on 8/3/26.
//

#ifndef MEDES_CURLPP_ADAPTER_H
#define MEDES_CURLPP_ADAPTER_H

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Infos.hpp>
#include <curlpp/Options.hpp>
#include "medes/medes.h"

namespace medes::curlpp_adapter {
    struct response_data {
        std::stringstream body;
        long status;
    };
}


template<>
struct medes::response_data_traits<medes::curlpp_adapter::response_data> {
    using body_type = decltype(curlpp_adapter::response_data::body);
    using status_type = decltype(curlpp_adapter::response_data::status);
};

namespace medes::curlpp_adapter {
    class curlpp_request_protocol {
    private:

    public:
        using request_data_type = request_data;
        using response_data_type = response_data;
        curlpp_request_protocol() = default;

        static response_data request(const request_data_type& request);
    };

    static_assert(RequestProtocol<curlpp_request_protocol>);
}

#endif //MEDES_CURLPP_ADAPTER_H
