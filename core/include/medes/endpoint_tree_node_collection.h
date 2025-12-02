//
// Created by backo on 17/12/25.
//

#ifndef MEDES_ENDPOINT_TREE_BACKPACK_H
#define MEDES_ENDPOINT_TREE_BACKPACK_H
#include "endpoint_tree_node.h"


namespace medes {
    template<internal::TypeStack... Paths>
    struct endpoint_tree_collection_info {
    };

    /**
      *
      * @tparam Paths Possible paths from the root to all leaves of the endpoint tree.
      */
    template<internal::TypeStack... Paths>
    struct basic_endpoint_tree_collection_info : protected internal::type_stack<Paths...> {
        using internal::type_stack<Paths...>::size;
        using internal::type_stack<Paths...>::empty;

        using main_type_stack = internal::type_stack<Paths...>;


        template<internal::TypeStack NewPath>
        using push_path = endpoint_tree_collection_info<NewPath, Paths...>;

        /**
         * Gets a path by an index, sorted by original template tree order.
         */
        template<int Index>
        using get_path_by_index = internal::get_type_from_pack<size - Index - 1, Paths...>;
    };

    template<internal::TypeStack CurrentPath, internal::TypeStack... Paths>
    struct endpoint_tree_collection_info<CurrentPath, Paths
                ...> : basic_endpoint_tree_collection_info<CurrentPath, Paths...> {
        using current_path = CurrentPath;
        using pop_path = endpoint_tree_collection_info<Paths...>;

        template<EndpointTree NewNode> requires (current_path::empty || !Endpoint<typename current_path::top>)
        using push_to_last_path = endpoint_tree_collection_info<typename CurrentPath::template push<NewNode>, Paths...>;
    };


    template<typename T>
    struct is_endpoint_tree_collection_info : std::false_type {
    };

    template<internal::TypeStack... Paths>
    struct is_endpoint_tree_collection_info<endpoint_tree_collection_info<Paths...>> : std::true_type {
    };

    template<>
    struct is_endpoint_tree_collection_info<endpoint_tree_collection_info<>> : std::true_type {
    };

    template<typename T>
    concept EndpointTreeCollectionInfo = is_endpoint_tree_collection_info<T>::value;

    using new_endpoint_tree_collection_info = endpoint_tree_collection_info<internal::type_stack<>>;
    static_assert(EndpointTreeCollectionInfo<new_endpoint_tree_collection_info>);

    // Collect

    template<EndpointTreeCollectionInfo Collection, EndpointTree FirstNode,
        EndpointTree ...RestNodes>
    struct tree_collect_all_impl {
        using original_path = Collection::current_path;
        using type = tree_collect_all_impl<typename FirstNode::template collect<Collection>::template push_path<
            original_path>, RestNodes...>::type;
    };


    template<EndpointTreeCollectionInfo Collection, EndpointTree FirstEndpoint>
    struct tree_collect_all_impl<Collection, FirstEndpoint> {
        using type = FirstEndpoint::template collect<Collection>;
    };


    template<EndpointTreeCollectionInfo Collection, EndpointTree Node, EndpointTree... Children>
    struct endpoint_tree_node_collect {
        using type = typename tree_collect_all_impl<typename Node::template collection_till_here<Collection>,
            Children...>::type;
    };

    template<EndpointTreeCollectionInfo Collection, EndpointTree Node> requires (Node::is_leaf)
    struct endpoint_tree_node_collect<Collection, Node> {
        using type = typename Node::template collection_till_here<Collection>;
    };

    // Strings join

    template<internal::TypeStack Path>
    struct endpoint_path_name_join {
        static constexpr internal::string_literal value =
                endpoint_path_name_join<typename Path::pop>::value + "." + Path::top::name;
    };


    template<internal::TypeStack Path> requires (Service<typename Path::pop::top>)
    struct endpoint_path_name_join<Path> {
        static constexpr internal::string_literal value = Path::top::name;
    };

    template<internal::TypeStack Path>
    struct endpoint_path_url_join {
        static constexpr internal::string_literal value = join_urls<endpoint_path_url_join<typename Path::pop>::value,
            Path::top::url>();
    };


    template<internal::TypeStack Path> requires (Service<typename Path::top>)
    struct endpoint_path_url_join<Path> {
        static constexpr internal::string_literal value = Path::top::url;
    };


    struct endpoint_info {
        std::string_view name, url;

        template<internal::TypeStack Path>
        constexpr static endpoint_info from_path() {
            return {endpoint_path_name_join<Path>::value, endpoint_path_url_join<Path>::value};
        }
    };


    /**
     * Gets the names and URLs of all paths in the collection, as an array of endpoint_info objects.
     * @tparam Collection /
     */
    template<EndpointTreeCollectionInfo Collection>
    struct collect_endpoints {
        static constexpr std::array value = internal::concatenate(
            collect_endpoints<typename Collection::pop_path>::value,
            std::array{endpoint_info::from_path<typename Collection::current_path>()});
    };

    template<>
    struct collect_endpoints<endpoint_tree_collection_info<>> {
        static constexpr std::array<endpoint_info, 0> value{};
    };


    template<EndpointTreeCollectionInfo Collection>
    static constexpr auto collect_endpoints_v = collect_endpoints<Collection>::value;

}

#endif //MEDES_ENDPOINT_TREE_BACKPACK_H
