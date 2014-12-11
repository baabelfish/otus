#pragma once

#include <tuple>
#include <limits>

namespace otus {

namespace internal {
    static constexpr size_t Max = std::numeric_limits<size_t>::max();

    // == Index of type ============================================================
    template<typename Target, class Tuple> struct index_of_type;
    template<typename Target>
        struct index_of_type<Target, std::tuple<>> {
            static constexpr size_t value = Max;
        };
    template<typename Target, typename... Rest>
        struct index_of_type<Target, std::tuple<Target, Rest...>> {
            static constexpr size_t value = 0;
        };
    template<typename Target, typename First, typename... Rest>
        struct index_of_type<Target, std::tuple<First, Rest...>> {
            static constexpr size_t next = index_of_type<Target, std::tuple<Rest...>>::value;
            static constexpr size_t value = next == Max ? Max : next + 1;
        };
} // namespace

namespace tmp {
    template<typename F, template<typename...> class Params, typename... Args, std::size_t... I>
        auto call_helper(F& f, Params<Args...>&& params, std::index_sequence<I...>) {
            return f(*std::get<I>(params)...);
        }
    template<typename F, template<typename...> class Params, typename... Args>
        auto call(F& f, Params<Args...>&& params) {
            return call_helper(f, std::move(params), std::index_sequence_for<Args...>{});
        }

    template<typename F, template<typename...> class Params, typename... Args, std::size_t... I>
        auto call_helper(F& f, size_t id, Params<Args...>&& params, std::index_sequence<I...>) {
            return f(id, *std::get<I>(params)...);
        }
    template<typename F, template<typename...> class Params, typename... Args>
        auto call(F& f, size_t id, Params<Args...>&& params) {
            return call_helper(f, id, std::move(params), std::index_sequence_for<Args...>{});
        }

    template<typename Target, class Tuple>
        struct index_of_type {
            static constexpr size_t value = internal::index_of_type<Target, Tuple>::value;
            static_assert(value != internal::Max, "No such type in provided tuple");
        };

    // == Index of type ============================================================
    template <typename Target, class Tuple>
        struct has_type {
            static constexpr bool value = internal::index_of_type<Target, Tuple>::value != internal::Max;
        };

    // == Tuple at index ============================================================
    template<typename T, typename... C>
        auto& tuple_at(std::tuple<C...>& t) {
            return std::get<index_of_type<T, typename std::remove_reference<decltype(t)>::type>::value>(t);
        }

    // == For each in tuple ==========================================================
    template<size_t I, typename F, template<typename...> class Tuple, typename... Args,
        typename std::enable_if<I == sizeof...(Args)>::type* = nullptr>
        void tuple_each(Tuple<Args...>&, F&) {}
    template<size_t I, typename F, template<typename...> class Tuple, typename... Args,
        typename std::enable_if<I < sizeof...(Args)>::type* = nullptr>
        void tuple_each(Tuple<Args...>& t, F f) {
            f(std::get<I>(t));
            tuple_each<I + 1, F>(t, f);
        }
    template<typename T, typename F>
        void tuple_each(T& t, F f) {
            tuple_each<0, F>(t, f);
        }

} // namespace tmp
} // namespace otus
