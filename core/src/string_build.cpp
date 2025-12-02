#include "medes/endpoint/string_build.h"

using namespace medes;

// ReSharper disable once CppDFAConstantFunctionResult
std::string medes::make_query(const std::map<std::string, std::string>& query_map) {
    std::string query_string = "?";
    int i = 0;
    for (const auto& [key, value] : query_map) {
        ++i;
        query_string.append(key);

        if (!value.empty()) query_string.append("=" + value);

        if (i != query_map.size()) query_string.push_back('&');
    }

    return query_string;
}

std::list<std::string> medes::make_headers(const std::multimap<std::string, std::string>& headers) {
    std::list<std::string> headers_list;
    for (auto& [k,v] : headers) {
        headers_list.push_back(k + ": " + v);
    }
    return headers_list;
}
