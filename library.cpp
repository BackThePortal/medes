#include "library.h"
#include "core/include/medes/service.h"

#include <iostream>

struct my_struct {
    int param1, param2;
};

[[nodiscard]] medes::path_params_static_map<"param1", "param2"> get_path_params(my_struct object) noexcept {
    return {object.param1, object.param2};
}

namespace medes::internal {
    template<>
    struct string_convert<my_struct> {
        static std::string make_string(my_struct&& s) {
            return std::to_string(s.param1) + " " + std::to_string(s.param2);
        }
    };
}


static_assert(medes::internal::ConvertibleToString<my_struct>);

using my_service = medes::service<"my_service", "/api/v1",
    medes::endpoint<medes::get, "hello_world", "/hello-world",
        medes::optional<"name"> // deduced type: std::string
    >,
    medes::endpoint<medes::get, "some_method", "/test",
        medes::required<"b", my_struct>
    >,

    medes::group<"post", "/post/{id}",
        medes::endpoint<medes::get, "get", "",
            medes::optional<"width", int>, medes::optional<"height", int>
        >,
        medes::endpoint<medes::patch, "update", "">,
        medes::endpoint<medes::_delete, "delete", "">
    >,
    medes::endpoint<medes::post, "publish_post", "/post">,
    medes::group<"admin", "/admin",
        medes::endpoint<medes::get, "get_some_info", "/info">
    >,
    medes::endpoint<medes::get, "hello", "/{param1}/hello/{param2}", medes::optional<"something", int>>
>;

using a_method = my_service::find_method<"get_post">;

void hello() {
    my_struct s{1, 2};
    std::cout << a_method(medes::path_params_list{"a", "b"}).url << std::endl;
    std::cout << my_service::find_method < "hello" > ::make<get_path_params>(s).url << std::endl;
    std::cout << "Hello, World!" << std::endl;


    my_service::request<"post.get">

    std::cout << (a_method::method::query_params::num);
    std::cout << (a_method::method::path_params::names.size()) << std::endl;
}


using new_service = medes::service<"new_service", "/api/v2",
    medes::get < "get_post", "/post/{id}", struct post>
>
