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
    class curlpp_request_protocol : request_protocol<request_data, raw_response_data> {
    private:
    public:
        curlpp_request_protocol() = default;

        raw_response_data request(const request_data_type& request_data_obj) override {
            raw_response_data response;
            curlpp::Easy request;
            request.setOpt(new curlpp::options::Url(request_data_obj.url));
            request.setOpt(new curlpp::options::HttpHeader(make_headers(request_data_obj.headers)));
            request.setOpt(new curlpp::options::WriteStream(&response.body));

            request.perform();

            response.status_code = curlpp::infos::ResponseCode::get(request);

            return response;
        };
    };
}

#endif //MEDES_CURLPP_ADAPTER_H
