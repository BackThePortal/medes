//
// Created by backo on 25/2/26.
//

#ifndef MEDES_CORE_ENDPOINT_TREE_NODE_CONCEPT_H
#define MEDES_CORE_ENDPOINT_TREE_NODE_CONCEPT_H

namespace medes {
    enum method_node_type {
        ROOT, // Service
        GROUP,
        ENDPOINT,
    };


    template<typename T>
    concept EndpointTree = requires
    {
        typename T::_endpoint_tree_tag;
        // T must also have a templated function find_method that takes an endpoint_tree_traversal_info non-type template parameter and
        // returns either a resolved_method or void. It must internally call find_method_impl.
        // No way to check this in a requirement yet.
    };

    template<typename T>
    concept Endpoint = EndpointTree<T> && T::node_type == ENDPOINT;
    template<typename T>
    concept Service = EndpointTree<T> && T::node_type == ROOT;
}
#endif //MEDES_CORE_ENDPOINT_TREE_NODE_CONCEPT_H