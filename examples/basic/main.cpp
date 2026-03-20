//
// Created by backo on 4/12/25.
//
#include <medes/medes.h>
#include <medes/adapters/curlpp_adapter.h>

using medes::curlpp_adapter::curlpp_request_protocol;

using service = medes::service<curlpp_request_protocol, "basic_service", "https://dummyjson.com/",
    medes::get<"test", "/test">,
    medes::group<"products", "/products",
        medes::get<"all", "">,
        medes::get<"get", "/{id}">,
        medes::get<"search", "search", medes::required<"q">, medes::optional<"limit", int>>
    >
>;

int main() {
    service service_instance;

    // Should print {"status":"ok","method":"GET"}
    std::cout << service_instance.request<"test">().body.str() << std::endl;

    // Should print a bunch
    std::cout << service_instance.request<"products.all">().body.str() << std::endl;
    std::cout << std::endl;
    std::cout << service_instance.request<"products.get">({}, 1).body.str() << std::endl;
    std::cout << std::endl;
    std::cout << std::endl;



    // This will produce a compile error
    //std::cout << service_instance.request<"products.search">({}).body.str() << std::endl;
    std::cout << service_instance.request<"products.search">({}, medes::path_params_list{}, medes::query<"q">("phone"),
        medes::query<"limit">(20)
        ).body.str() << std::endl;
}