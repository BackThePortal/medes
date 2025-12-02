//
// Created by backo on 4/11/25.
//

#ifndef MEDES_META_H
#define MEDES_META_H

#include <type_traits>
#include <tuple>
#include <utility>
#include <string>

// Source - https://stackoverflow.com/a/69696852
// Posted by Jean-Michaël Celerier
// Retrieved 2025-12-17, License - CC BY-SA 4.0
#define CONCEPT_AS_LAMBDA(TheConcept) \
[] <typename T> () consteval { return TheConcept<T>; }


namespace medes::internal {
    template<typename... Types>
    struct pack {
    };

    template<auto Value>
    using get_type_from_value = std::remove_cvref_t<decltype(Value)>;

    template<std::size_t N, typename... Ts>
    using get_type_from_pack = std::tuple_element_t<N, std::tuple<Ts...>>;

    template<typename T, typename... Args>
    struct is_friend_constructible {
    private:
        template<typename U, typename = decltype(U(std::declval<Args>()...))>
        static std::true_type test(int) {
            return {};
        };

        template<typename>
        static std::false_type test(...) {
            return {};
        };

    public:
        static constexpr bool value = decltype(test<T>(0))::value;
    };

    template<typename T, typename... Args>
    constexpr bool is_friend_constructible_v = is_friend_constructible<T, Args...>::value;

    template<typename T>
    std::add_rvalue_reference<T>::type declval();


    // Concepts


    // Source - https://stackoverflow.com/a/71563534
    template<class T, class U>
    concept explicitly_convertible_to = requires(T t) { static_cast<U>(t); };


    template<auto F, typename... Args>
    concept ConstexprCallableWith = requires(Args&&... args)
    {
        { []<typename... Ts>(Ts&&... xs) consteval { F(std::forward<Ts>(xs)...); }(std::forward<Args>(args)...) };
    };


    template<typename T>
    struct is_tuple : std::false_type {
    };

    template<typename... Ts>
    struct is_tuple<std::tuple<Ts...>> : std::true_type {
    };

    template<typename T>
    concept Tuple = is_tuple<T>::value;

    template<typename T>
    struct array_info {
        using type = void;
    };

    template<typename T, size_t N>
    struct array_info<std::array<T, N>> {
        using type = T;
        static constexpr size_t size = N;
    };

    template<typename T>
    concept StdArray = !std::is_same_v<typename array_info<T>::type, void>;

    template<typename...>
    struct array_pack_info;

    template<StdArray FirstArray, StdArray... Arrays>
    struct array_pack_info<FirstArray, Arrays...> {
        static constexpr bool all_same_type = (std::is_same_v<typename array_info<FirstArray>::type, typename array_info<Arrays>::type> && ...);
        static constexpr std::size_t total_size = array_info<FirstArray>::size + (array_info<Arrays>::size + ...);
        using type = array_info<FirstArray>::type;
    };

    template<>
    struct array_pack_info<> {
        static constexpr bool all_same_type = true;
        static constexpr std::size_t total_size = 0;
        using type = void;
    };

    template<StdArray... Arrays>
    constexpr bool array_same_type_v = array_pack_info<Arrays...>::all_same_type;


    // Arrays
    template<StdArray... Arrays> requires (sizeof...(Arrays) > 0) && array_same_type_v<Arrays...>
    constexpr auto concatenate(const Arrays&... arrays)  {
        using arrays_info = array_pack_info<Arrays...>;
        std::array<typename arrays_info::type, arrays_info::total_size> result;
        std::size_t index = 0;
        ((std::copy(arrays.begin(), arrays.end(), result.begin() + index), index += arrays.size()), ...);
        return result;
    }
}

#endif //MEDES_META_H
