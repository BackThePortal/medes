#ifndef MEDES_STRING_BUILD_H
#define MEDES_STRING_BUILD_H

#include <list>
#include <map>
#include <string>

namespace medes {
    /**
       * @brief Create a query string from a map of query keys and values. The returned string starts with '?'. If a query
       * value is empty, the equals sign '=' is not included in the query string.
       */
    std::string make_query(const std::map<std::string, std::string>& query_map);

    std::list<std::string> make_headers(const std::multimap<std::string, std::string>& headers);
}
#endif
