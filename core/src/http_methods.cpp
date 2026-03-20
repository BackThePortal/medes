#include "medes/http_methods.h"

std::string medes::http_method_to_str(const http_method method) noexcept {
    switch (method) {
        case http_method::get:
            return "GET";
        case http_method::post:
            return "POST";
        case http_method::put:
            return "PUT";
        case http_method::patch:
            return "PATCH";
        case http_method::_delete:
            return "DELETE";
        default:
            std::unreachable();
    }
}
