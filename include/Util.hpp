#pragma once

#include <iterator>
#include <tuple>
#include <utility>

namespace tel {
template <typename Container>
std::size_t size_in_bytes(const Container& container) {
    return std::size(container) * (sizeof(Container::value_type));
}

template <typename UnaryFunction, typename... Types>
constexpr bool
all_of(UnaryFunction&& p,
       std::tuple<Types...>&& t) noexcept(std::conjunction_v<std::is_nothrow_invocable<UnaryFunction, Types>...>) {
    return std::apply([&]<typename... T0>(T0&&... xs) constexpr { return (p(std::forward<T0>(xs)) && ...); },
                      std::forward<std::tuple<Types...>>(t));
}

template <typename UnaryFunction, typename... Types>
constexpr std::enable_if_t<std::conjunction_v<std::is_same<std::invoke_result_t<UnaryFunction, Types>, bool>...>, bool>
any_of(UnaryFunction&& p,
       std::tuple<Types...>&& t) noexcept(std::conjunction_v<std::is_nothrow_invocable<UnaryFunction, Types>...>) {
    return std::apply([&]<typename... T0>(T0&&... xs) constexpr { return (p(std::forward<T0>(xs)) || ...); },
                      std::forward<std::tuple<Types...>>(t));
}

template <typename UnaryFunction, typename... Types>
constexpr std::enable_if_t<std::conjunction_v<std::is_same<std::invoke_result_t<UnaryFunction, Types>, bool>...>, bool>
none_of(UnaryFunction&& p,
        std::tuple<Types...>&& t) noexcept(std::conjunction_v<std::is_nothrow_invocable<UnaryFunction, Types>...>) {
    return std::apply([&]<typename... T0>(T0&&... xs) constexpr { return !(p(std::forward<T0>(xs)) || ...); },
                      std::forward<std::tuple<Types...>>(t));
}

template <typename... LHS, typename... RHS, typename BinaryFunc>
std::tuple<std::invoke_result_t<BinaryFunc, LHS, RHS>...> element_wise(std::tuple<LHS...> lhs, std::tuple<RHS...> rhs,
                                                                       BinaryFunc func) {
    using Indices = std::index_sequence_for<LHS...>;
    return std::make_tuple(func(std::get<Indices>(lhs), std::get<Indices>(rhs))...);
}
} // namespace tel