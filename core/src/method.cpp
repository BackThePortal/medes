//
// Created by backo on 5/11/25.
//
#include <iostream>
#include "medes/medes.h"
#include "medes/path.h"
using namespace medes;

enum class A {};

template<>
struct medes::internal::string_convert<A> {
    static std::string make_string(A&&) {
        return "A";
    }
};


using my_service = medes::service<medes::dummy_request_protocol, "my_service", "https://dummyjson.com/",
    medes::get<"fortune", "/fortune", medes::optional<"words", A>>,
    medes::group<"posts", "/posts",
        medes::get<"get", "/get/{id}">
    >
>;




using hmmm = my_service::tree;
static_assert(my_service::tree::size == 2);

static constexpr auto valuea = collect_endpoints<endpoint_tree_collection_info<>>::value;

static_assert(my_service::endpoint_exists_v<"posts.get">);
using my_endpoint = my_service::find_endpoint_t<internal::string_literal("posts.get")>;
using my_resolved_endpoint = my_service::resolve_endpoint<"posts.get">;
constexpr auto my_endpoint2 = my_service::find_endpoint_info<"posts.get">;


static_assert(std::is_same_v<my_endpoint, medes::get<"get", "/get/{id}">>);
static constexpr auto name = endpoint_path_name_join<my_service::tree::current_path>::value;
static constexpr auto url = endpoint_path_url_join<my_service::tree::current_path>::value;

static_assert(name == "posts.get");

int main() {
    my_service service_instance;
    service_instance.request<"posts.get">({}, path_params_list{2})([](const raw_response_data& response_data) {
        std::cout << response_data.body.str() << std::endl;
    });
   // Post p = co_await my_service::request<"posts.get">(path_params_list{20});
}
/*

using my_method = endpoint<http_method::get, "get_image", "/image/{id}/{name}", required<"size", int>, optional<"upscale", bool>>;
using my_method2 = endpoint<http_method::get, "get_image", "/image", required<"size", int>, optional<"upscale", bool>>;
int main() {
     auto b = my_method::make(path_params_list{30, "a"}, query<"size">(1));
     auto b2 = my_method2::make(query<"size">(5));
     std::map<std::string, std::string> params;
     path_params_map params_map(params);
     params_map["id"] = "aaa";
}
*/