//
// Created by backo on 5/11/25.
//

#ifndef MEDES_GROUP_H
#define MEDES_GROUP_H

#include <string>
#include "internal/string_literal.h"
#include "endpoint_tree_node.h"
#include "internal/meta.h"

namespace medes {



    template<typename TDerived, internal::string_literal Name, internal::string_literal BaseUrl, EndpointTree...
        MethodsSubtrees> requires (sizeof...(MethodsSubtrees) > 0)
    class basic_group : public endpoint_tree_node<TDerived, Name, BaseUrl, method_node_type::GROUP, MethodsSubtrees...> {
    public:
        using endpoint_tree_node_class = endpoint_tree_node<TDerived, Name, BaseUrl, method_node_type::GROUP>;

/*
        template<endpoint_tree_traversal_info TraversalInfo>
        using find_endpoint = method_tree_node_class::template find_endpoint_impl<TraversalInfo, MethodsSubtrees
            ...>::type;
*/
    };

    template<internal::string_literal Name, internal::string_literal BaseUrl, EndpointTree... MethodsSubtrees>
    class group : public basic_group<group<Name, BaseUrl, MethodsSubtrees...>, Name, BaseUrl, MethodsSubtrees...> {
    };
}
#endif //MEDES_GROUP_H
