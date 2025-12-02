//
// Created by backo on 17/12/25.
//

#ifndef MEDES_STATIC_STACK_H
#define MEDES_STATIC_STACK_H
#include <cstddef>
#include <tuple>

namespace medes::internal {
    /*
     * TODO Make type_stack (or create a new one that does) support a lambda as a template
     * parameter that checks if a given type can be put in the stack or not. This could be
     * used with the macro CONCEPT_AS_LAMBDA to allow only types that satisfy a certain
     * concept to be pushed to the stack.
     */

    template<typename... Types>
    struct type_stack;

    // Base struct for data fields and push action.
    template<typename... Types>
    struct type_stack_base {
        static constexpr std::size_t size = sizeof...(Types);
        static constexpr bool empty = size == 0;

        using as_tuple = std::tuple<Types...>;


        template<typename NewType>
        using push = type_stack<NewType, Types...>;
    };

    // Main template, only really used for empty type stacks.
    template<typename... Types>
    struct type_stack : type_stack_base<Types...> {
    };


    // Specialized template for type stacks with at least one type.
    template<typename Type, typename... Types>
    struct type_stack<Type, Types...> : type_stack_base<Type, Types...> {
        /**
         * A new type_stack with all the types in this type_stack except the first one.
         */
        using pop = type_stack<Types...>;

        /// A new type_stack with the first element being replaced by NewType.
        template<typename NewType>
        using replace = type_stack<NewType, Types...>;

        using top = Type;
    };

    template<typename T>
    struct is_type_stack : std::false_type {
    };

    template<typename... Types>
    struct is_type_stack<type_stack<Types...>> : std::true_type {
    };

    template<typename T>
    concept TypeStack = is_type_stack<T>::value;

}

#endif //MEDES_STATIC_STACK_H
