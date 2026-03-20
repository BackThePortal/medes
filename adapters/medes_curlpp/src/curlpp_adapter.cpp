//
// Created by backo on 8/3/26.

#include "medes/adapters/curlpp_adapter.h"



medes::curlpp_adapter::response_data medes::curlpp_adapter::curlpp_request_protocol::request(
    const request_data_type& request) {
    response_data response;
    curlpp::Easy request_handle;

    std::cout << "Sending request to " << request.url << std::endl;

    request_handle.setOpt(new curlpp::options::Url(request.url));
    request_handle.setOpt(new curlpp::options::HttpHeader(make_headers(request.headers)));
    if (!request.body.empty()) {
        request_handle.setOpt(new curlpp::options::PostFields(request.body));
    } else if (request.method != http_method::get) {
        request_handle.setOpt(new curlpp::options::CustomRequest(http_method_to_str(request.method)));
    } // The default method is GET

    request_handle.setOpt(new curlpp::options::WriteStream(&response.body));

    // Follow redirects
    request_handle.setOpt(new curlpp::options::FollowLocation(true));

    request_handle.perform();

    response.status = curlpp::infos::ResponseCode::get(request_handle);

    return response;
}
