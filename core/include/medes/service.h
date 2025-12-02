//
// Created by backo on 5/11/25.
//

#ifndef MEDES_SERVICE_H
#define MEDES_SERVICE_H

#include "internal/string_literal.h"
#include "endpoint_tree_node.h"
#include "endpoint/endpoint.h"
#include "request_protocol.h"
#include "group.h"

namespace medes {
    template<Service TService, internal::string_literal MethodName>
    constexpr bool method_exists_v = !std::is_same_v<typename TService::template find_endpoint<MethodName>, void>;

    template<RequestProtocolConcept TRequestProtocol, internal::string_literal Name, internal::string_literal BaseUrl,
        EndpointTree... MethodSubtrees> requires (sizeof...(MethodSubtrees) > 0)
    class service : public endpoint_tree_node<service<TRequestProtocol, Name, BaseUrl, MethodSubtrees...>, Name, BaseUrl
                , ROOT, MethodSubtrees...> {
    public:
        using service_class = service<TRequestProtocol, Name, BaseUrl, MethodSubtrees...>;
        using endpoint_tree_node_class = endpoint_tree_node<service_class, Name, BaseUrl, method_node_type::ROOT,
            MethodSubtrees...>;

        using typename endpoint_tree_node_class::_endpoint_tree_tag;

        using tree = endpoint_tree_node_class::template collect<new_endpoint_tree_collection_info>;
        static constexpr std::size_t endpoint_count = tree::size;

        static constexpr auto endpoints_array = collect_endpoints_v<tree>;

    private:
        template<internal::string_literal TargetName>
        static constexpr auto find_it_endpoint_v = std::find_if(endpoints_array.begin(), endpoints_array.end(),
                                                                [](const endpoint_info& other) {
                                                                    return other.name == std::string_view(TargetName);
                                                                });

        template<internal::string_literal TargetName>
        struct find_endpoint_impl {
        private:
            static constexpr auto iterator = find_it_endpoint_v<TargetName>;

        public:
            static constexpr bool found = iterator != endpoints_array.end();
            static constexpr auto index = std::distance(endpoints_array.begin(), iterator);
            static constexpr struct endpoint_info endpoint_info = found ? *iterator : (struct endpoint_info){};
            using endpoint_type = tree::template get_path_by_index<index>::top;

            static constexpr auto literal_url = endpoint_path_url_join<typename tree::template get_path_by_index<
                index>>::value;
        };

    public:
        template<internal::string_literal TargetName>
        static constexpr auto endpoint_exists_v = find_it_endpoint_v<TargetName> != endpoints_array.end();

        template<internal::string_literal TargetName> requires (endpoint_exists_v<TargetName>)
        static constexpr auto find_endpoint_info = find_endpoint_impl<TargetName>::endpoint_info;

        // For some reason the constraint is always false in CLion's analyzer.
#ifdef  __CLION_IDE__
        template<internal::string_literal TargetName> requires true
#else
        template<internal::string_literal TargetName> requires (endpoint_exists_v<TargetName>)



#endif
        using find_endpoint_t = find_endpoint_impl<TargetName>::endpoint_type;


#ifdef  __CLION_IDE__
        template<internal::string_literal TargetName> requires true
#else
        template<internal::string_literal TargetName> requires (endpoint_exists_v<TargetName>)



#endif
        using resolve_endpoint = resolved_endpoint<find_endpoint_impl<TargetName>::literal_url, find_endpoint_t<
            TargetName>>;

        template<internal::string_literal TargetName, typename... Ts> requires (
#ifdef  __CLION_IDE__
            true
#else
            endpoint_exists_v<TargetName>
#endif
        )
        auto request(partial_request_data&& partial_request_data, Ts&&... resolved_endpoint_args) {
            using endpoint = find_endpoint_impl<TargetName>;
            using resolved_endpoint = resolve_endpoint<TargetName>;

            // TODO: move request logic to resolved_endpoint
            std::string url = resolved_endpoint::make(resolved_endpoint_args...).url;
            request_data data{endpoint::endpoint_type::httpMethod, std::move(url), std::move(partial_request_data)};

            if constexpr (std::is_same_v<typename TRequestProtocol::request_arg, void>) {
                return this->request_protocol->request(data);
            } else {
                return [this, data = std::move(data)](typename TRequestProtocol::request_arg&& arg) {
                    return this->request_protocol->request(data, arg);
                };
            }
        }

        std::shared_ptr<TRequestProtocol> request_protocol;
        template<typename... Ts>
        explicit service(Ts... args) : request_protocol(std::make_shared<TRequestProtocol>(args...)) {

        }

        explicit service(std::shared_ptr<TRequestProtocol> request_protocol_p) : request_protocol(request_protocol_p) {}

    private:
        //template<internal::Tuple PreviousEndpointIds,>

        /*
        template<internal::string_literal TargetName>
        using find_endpoint = group_class::template find_endpoint<endpoint_tree_traversal_info<TargetName, service_class>>;
        template<internal::string_literal MethodName>
            requires method_exists_v<service_class, MethodName>
        void request(auto&... args) {
            using target_method = find_method<MethodName>;
        }
        */
    };
}

#endif //MEDES_SERVICE_H
