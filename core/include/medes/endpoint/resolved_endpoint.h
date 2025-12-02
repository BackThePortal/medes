//
// Created by backo on 23/11/25.
//

#ifndef MEDES_resolved_endpoint_H
#define MEDES_resolved_endpoint_H

#include "medes/internal/string_literal.h"
#include "medes/endpoint_tree_node.h"
#include "path_params.h"
#include "query_params.h"

namespace medes {
    template<internal::string_literal ResolvedURL, Endpoint TargetMethod>
    class resolved_endpoint_base {
    private:

    public:
        explicit resolved_endpoint_base(std::string url) : url(std::move(url)) {
        };
        using method = TargetMethod;
        static constexpr internal::string_literal baseUrl = ResolvedURL;

        std::string url;
    };

    namespace detail {
        template<typename T>
        struct is_resolved_endpoint : std::bool_constant<false> {
        };

        template<internal::string_literal URL, Endpoint TargetEndpoint>
        struct is_resolved_endpoint<resolved_endpoint_base<URL, TargetEndpoint>> : std::bool_constant<true> {
        };
    }

    template<typename T>
    concept ResolvedEndpoint = detail::is_resolved_endpoint<T>::value;

    template<internal::string_literal ResolvedURL, Endpoint TargetEndpoint>
    class resolved_endpoint : resolved_endpoint_base<ResolvedURL, TargetEndpoint> {
    public:
        using resolved_endpoint_base_class = resolved_endpoint_base<ResolvedURL, TargetEndpoint>;
        using resolved_endpoint_base_class::url;

        using path_params = path_params_handler<ResolvedURL>;
        using query_params = TargetEndpoint::query_params;
        explicit resolved_endpoint(std::string&& url) : resolved_endpoint_base_class(std::move(url)) {}


        template<typename ContainerOrClass, QueryValue... TargetQueryKeys>
        static resolved_endpoint
        make(const ContainerOrClass& path_params_obj, TargetQueryKeys... query_keys_raw) {
            return resolved_endpoint(path_params::template validate_and_make<ContainerOrClass>(path_params_obj) +
                                     query_params::template validate_and_make<TargetQueryKeys...>(query_keys_raw...));
        }


        template<typename PathParamsClass, auto Func, QueryValue... TargetQueryKeys>
        static resolved_endpoint
        make(const PathParamsClass& object, TargetQueryKeys... query_keys_raw) {
            return resolved_endpoint(path_params::template validate_and_make<PathParamsClass, Func>(object) +
                                     query_params::template validate_and_make<TargetQueryKeys...>(query_keys_raw...));
        }

        template<QueryValue... TargetQueryKeys>
        static resolved_endpoint make(TargetQueryKeys... query_keys_raw) {
            return make(path_params_list{}, query_keys_raw...);
        }
        template<internal::ConvertibleToString... Ts>
        static resolved_endpoint make(Ts&&... values) {
            return make(path_params::template validate_and_make<>(path_params_list(internal::string_convert<Ts>::make_string(std::forward<Ts>(values))...)));
        }



    };
}
#endif //MEDES_resolved_endpoint_H
