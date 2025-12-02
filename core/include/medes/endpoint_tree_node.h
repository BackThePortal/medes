//
// Created by backo on 5/11/25.
//

#ifndef MEDES_ENDPOINT_TREE_NODE_H
#define MEDES_ENDPOINT_TREE_NODE_H

#include <string>
#include <utility>
#include "internal/string_literal.h"
#include "internal/meta.h"
#include "path.h"
#include "request_protocol.h"
#include "internal/type_stack.h"
#include "utilities/url.h"
#include "endpoint_tree_node_concept.h"
#include "endpoint_tree_node_collection.h"

namespace medes {
    /*
        namespace detail {
            template<Endpoint TEndpoint, internal::string_literal ResolvedURL, internal::string_literal
                TargetName>
            struct endpoint_tree_traversal_info_resolve_endpoint_impl {
                using type = void;
            };

            template<Endpoint TEndpoint, internal::string_literal ResolvedURL, internal::string_literal
                TargetName> requires (
                TargetName == TEndpoint::name)
            struct endpoint_tree_traversal_info_resolve_endpoint_impl<TEndpoint, ResolvedURL, TargetName> {
                using type = TEndpoint::template resolved_endpoint<ResolvedURL>;
            };

            template<typename>
            struct endpoint_tree_node_stack_url_join_impl {
                static constexpr internal::string_literal value = "";
            };

            template<Endpoint FirstEndpoint, Endpoint... RestEndpoints> requires (sizeof...(RestEndpoints) > 0)
            struct endpoint_tree_node_stack_url_join_impl<internal::type_stack<FirstEndpoint, RestEndpoints...>> {
                static constexpr internal::string_literal value = join_urls<FirstEndpoint::url,
                    endpoint_tree_node_stack_url_join_impl<internal::type_stack<RestEndpoints...>>::value>();
            };

            template<Endpoint Endpoint>
            struct endpoint_tree_node_stack_url_join_impl<internal::type_stack<Endpoint>> {
                static constexpr internal::string_literal value = Endpoint::url;
            };
        }

        template<internal::string_literal TargetName, Service TService, internal::TypeStack Nodes = internal::type_stack<>>
        struct endpoint_tree_traversal_info {
            static constexpr auto resolved_url = detail::endpoint_tree_node_stack_url_join_impl<Nodes>::value;
            using service_class = TService;
            template<Endpoint TEndpoint>
            using match = detail::endpoint_tree_traversal_info_resolve_endpoint_impl<TEndpoint, resolved_url,
                TargetName>::type;

            template<EndpointTree NewNode> requires (Nodes::size == 0 || !Endpoint<typename Nodes::top>)
            using push = endpoint_tree_traversal_info<TargetName, TService, typename Nodes::template push<NewNode>>;

            static constexpr auto target_name = TargetName;
        };
        */


    template<typename TDerived, internal::string_literal Name, internal::string_literal BaseURL, method_node_type
        NodeType, EndpointTree...
        Children>
    class endpoint_tree_node {
    public:
        struct [[maybe_unused]] _endpoint_tree_tag {
        };


        static constexpr bool is_leaf = NodeType == ENDPOINT;
        static constexpr std::size_t children = sizeof...(Children);

        static_assert(!is_leaf || (sizeof...(Children) == 0), "An endpoint can't have children.");

    protected:
        /*
        template<endpoint_tree_traversal_info TraversalInfo, EndpointTree First,
            EndpointTree... Rest>
        struct find_endpoint_impl {
            using type = std::remove_reference_t<decltype([] -> decltype(auto) {
                using first_lookup_result = First::template find_endpoint<TraversalInfo>;
                if constexpr (!std::is_void_v<first_lookup_result>) {
                    return internal::declval<first_lookup_result>();
                } else if constexpr (sizeof...(Rest) > 0) {
                    return internal::declval<typename find_endpoint_impl<TraversalInfo, Rest...>::type>();
                } else /* if constexpr (NodeType == ROOT) {
                    static_assert(false, "Method not found");
                } else  {
                    return internal::declval<void>();
                }
            }())>;
        };

        template<internal::TypeStack Nodes, EndpointTree First, EndpointTree... Rest>
        struct collect_impl {
            using type = std::remove_reference_t<decltype(([] -> decltype(auto) {
                if constexpr (sizeof...(Rest) == 0) {
                    return First::template collect<Nodes>();
                } else {
                    return internal::declval<typename collect_impl<typename Nodes::template push<First>, Rest
                        ...>::type>();
                }
            })())>;
        };
        */
        template<EndpointTreeCollectionInfo Collection>
        using collection_till_here = Collection::template push_to_last_path<TDerived>;

        template<EndpointTreeCollectionInfo Collection, EndpointTree Node, EndpointTree... _Children>
        friend struct endpoint_tree_node_collect;

    public:
        template<EndpointTreeCollectionInfo Collection>
        using collect = endpoint_tree_node_collect<Collection, TDerived, Children...>::type;

        static constexpr auto name = Name;
        static constexpr auto url = BaseURL;
        static constexpr method_node_type node_type = NodeType;

        static_assert(Name.count('.') == 0, "Name can't contain dots.");
    };
}
#endif //MEDES_ENDPOINT_TREE_NODE_H
