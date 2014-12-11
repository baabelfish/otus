#pragma once

#include "tmp.hpp"
#include "helpers.hpp"
#include <tuple>
#include <bitset>

namespace otus {
template<typename... Components>
    struct EntityHelper {
        struct Entity {
            const size_t id;
            const std::bitset<sizeof...(Components)> mask;
            std::tuple<Components*...> components;

            Entity(std::bitset<sizeof...(Components)> bm):
                id(helpers::uniqueId<EntityHelper<Components...>>()),
                mask(bm) {
            }

            virtual ~Entity() {}
            template<typename C>
            auto& component() {
                return *tmp::tuple_at<C*>(components);
            }
        };

        template<typename... Args>
            struct EntityConstructor : public Entity {
                static void init(std::tuple<Components*...>&) {}

                template<typename C>
                static void init(std::tuple<Components*...>& cc) {
                    tmp::tuple_at<C*>(cc) = new C();
                }

                template<typename First, typename... Rest,
                    typename std::enable_if<sizeof...(Rest) >= 1>::type* = nullptr>
                static void init(std::tuple<Components*...>& cc) {
                    init<First>(cc);
                    init<Rest...>(cc);
                }

                EntityConstructor():
                    Entity(Helpers<Components...>::template bitmaskFromVarargs<Args...>()) {
                    init<Args...>(Entity::components);
                }
            };
    };
} // namespace otus
