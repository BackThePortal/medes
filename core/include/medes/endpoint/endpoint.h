//
// Created by backo on 5/11/25.
//

#ifndef MEDES_ENDPOINT_H
#define MEDES_ENDPOINT_H

#include "medes/internal/singleton.h"
#include "medes/internal/string_literal.h"
#include "medes/http_methods.h"
#include "query_params.h"
#include "path_params.h"
#include "resolved_endpoint.h"
#include "medes/endpoint_tree_node.h"

namespace medes {
    template<typename TDerived, http_method HttpMethod, internal::string_literal Name, internal::string_literal
        LiteralURL,
        QueryKeyDescr... QueryKeys>
    class basic_endpoint
            : public endpoint_tree_node<TDerived, Name, "", method_node_type::ENDPOINT>
            //public singleton<endpoint<HttpMethod, Name, LiteralURL, QueryKeys...>, SingletonInstanceType::REFERENCE>
    {
    private:
        basic_endpoint() = default;

    public:
        static_assert(LiteralURL.size() != 0, "URL may not be empty");
        static_assert(LiteralURL.back() != '/', "URL may not end with a slash");

        using query_params = query_params_handler<QueryKeys...>;

        using endpoint_tree_class = endpoint_tree_node<TDerived, Name, "", method_node_type::ENDPOINT>;

        static constexpr http_method httpMethod = HttpMethod;
        static constexpr internal::string_literal url = LiteralURL;


        template<internal::string_literal ResolvedURL>
        using resolved_endpoint = resolved_endpoint<ResolvedURL, TDerived>;


        /**
         * @returns A resolved_endpoint template instantiation with this endpoint as a template argument if
         * TraversalInfo::TargetName == Name; void otherwise.
        template<endpoint_tree_traversal_info TraversalInfo>
        using find_endpoint = decltype(TraversalInfo)::template match<Name>::type;
         */
    };

    template<internal::string_literal Name, internal::string_literal LiteralURL, QueryKeyDescr... QueryKeys>
    class get : public basic_endpoint<get<Name, LiteralURL, QueryKeys...>, http_method::get, Name, LiteralURL, QueryKeys
                ...> {
    };

    template<internal::string_literal Name, internal::string_literal LiteralURL, QueryKeyDescr... QueryKeys>
    class post : public basic_endpoint<post<Name, LiteralURL, QueryKeys...>, http_method::post, Name, LiteralURL, QueryKeys
                ...> {
    };

}


#endif //MEDES_ENDPOINT_H
