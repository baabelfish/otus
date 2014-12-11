#pragma once

#include "tmp.hpp"
#include <tuple>
#include <bitset>
#include <type_traits>

namespace otus {
namespace helpers {
    template<typename T>
        size_t uniqueId() {
            static size_t id = 0;
            return id++;
        }
} // namespace helpers

    template<typename... Components>
        struct Helpers {
            static std::hash<std::bitset<sizeof...(Components)>> HashF;

            template<size_t I, typename... Args,
                typename std::enable_if<I == sizeof...(Args)>::type* = nullptr>
                inline static void populateParams(const std::tuple<Components*...>&, std::tuple<Args...>&) {}

            template<size_t I, typename... Args,
                typename std::enable_if<I < sizeof...(Args)>::type* = nullptr>
                inline static void populateParams(const std::tuple<Components*...>& from, std::tuple<Args...>& to) {
                    using T = typename std::remove_reference<decltype(std::get<I>(to))>::type;
                    std::get<I>(to) = std::get<tmp::index_of_type<T, typename std::tuple<Components*...>>::value>(from);
                    populateParams<I + 1, Args...>(from, to);
                }

            template<size_t Index,
                typename... Args,
                typename std::enable_if<Index == sizeof...(Components)>::type* = nullptr>
                static inline auto bitmaskFromVarargs() {
                    return std::bitset<sizeof...(Components)>();
                }

            template<size_t Index,
                typename... Args,
                typename std::enable_if<Index != sizeof...(Components)>::type* = nullptr>
                static inline auto bitmaskFromVarargs() {
                    using ComponentType = typename std::tuple_element<Index, std::tuple<Components...>>::type;
                    using EntityTypes = typename std::tuple<Args...>;
                    auto mask = bitmaskFromVarargs<Index + 1, Args...>();
                    mask[Index] = tmp::has_type<ComponentType, EntityTypes>::value;
                    return mask;
                }

            template<typename... Args>
                static inline auto bitmaskFromVarargs() {
                    return bitmaskFromVarargs<0, Args...>();
                }
            };

} // namespace otus
