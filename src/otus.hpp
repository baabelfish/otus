#pragma once

#include "entity.hpp"
#include "events.hpp"
#include "helpers.hpp"
#include "tmp.hpp"
#include "eventmanager.hpp"
#include <algorithm>
#include <cassert>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

namespace otus {

template<typename... Components>
    class ES {
    public:
        struct System;
        using Entity = typename EntityHelper<Components...>::Entity;
        static const std::hash<std::bitset<sizeof...(Components)>> hashf;

        ES():
            entities(),
            entities_by_hash(),
            systems() {}
        virtual ~ES();

        template<typename F> void to(size_t id, F f);
        template<typename S, typename... CArgs> void addSystem(CArgs&&... args);
        template<typename... Args> size_t add();
        template<typename... Args, typename F> size_t add(F f);
        template<typename... Params> void update(Params... params);
        void debug() const;
        void each(std::function<void(size_t)> f);
        void remove(size_t id);
        size_t size() const;

        template<template<typename...> class T, typename ID, typename... Args, typename F>
            void _each(T<ID, Args...>*, F f) {
                each<Args...>(f);
            }

        template<typename F>
            void each(F f) {
                using FT = tmp::function_traits<F>;
                typename FT::param_types* t = nullptr;
                _each(t, f);
            }

        // TODO: get the definition out of here
        template<typename... Args, typename F,
            typename std::enable_if<sizeof...(Args) >= 1>::type* = nullptr>
            void each(F f) {
                auto smask = Helpers<Components...>::template bitmaskFromVarargs<Args...>();
                auto hash = hashf(smask);
                auto ecache = entities_by_hash.find(hash);
                auto c = [&](Entity* e) {
                    std::tuple<Args*...> params;
                    Helpers<Components...>::template populateParams<0>(e->components, params);
                    tmp::apply(f, e->id, std::move(params));
                };
                if (ecache != entities_by_hash.end())
                    for (auto& e : ecache->second)
                        c(e);
                else
                    for (auto& e : entities)
                        if ((e.second->mask & smask) == smask)
                            entities_by_hash[hash].push_back(e.second),
                            c(e.second);
            }

    private:
        std::map<size_t, Entity*> entities;
        std::unordered_map<size_t, std::vector<Entity*>> entities_by_hash;
        std::vector<System*> systems;
        EventManager em;

        template<typename... Args, typename F> void to(Entity* e, F f);
        template<typename... Args> auto _add();
        template<template<typename...> class T, typename... Args, typename F>
            void _to(T<Args...>*, size_t id, F f);
    };

template<typename... Components>
const std::hash<std::bitset<sizeof...(Components)>> ES<Components...>::hashf{};



// ================================================================================
template<typename... Components>
    ES<Components...>::~ES() {
        for (auto x : entities) delete x.second;
        for (auto x : systems) delete x;
    }

template<typename... Components>
template<typename... Args, typename F>
    void ES<Components...>::to(Entity* e, F f) {
        auto smask = Helpers<Components...>::template bitmaskFromVarargs<Args...>();
        if ((e->mask & smask) == smask) {
            std::tuple<Args*...> params;
            Helpers<Components...>::template populateParams<0>(e->components, params);
            tmp::apply(f, std::move(params));
        }
    }

template<typename... Components>
template<typename F>
    void ES<Components...>::to(size_t id, F f) {
        using FT = tmp::function_traits<F>;
        typename FT::param_types* t = nullptr;
        _to(t, id, f);
    }

template<typename... Components>
template<template<typename...> class T, typename... Args, typename F>
    void ES<Components...>::_to(T<Args...>*, size_t id, F f) {
        auto e = entities.find(id);
        if (e != entities.end()) {
            to<Args...>(e->second, f);
        }
    }

template<typename... Components>
template<typename... Params>
    void ES<Components...>::update(Params... params) {
        for (auto&& s : systems)
            s->update(std::forward<Params>(params)...);
    }

template<typename... Components>
template<typename S, typename... CArgs>
    void ES<Components...>::addSystem(CArgs&&... args) {
        auto system = new S(std::forward<CArgs>(args)...);
        system->god = this;
        system->configure();
        systems.emplace_back(std::move(system));
    }

template<typename... Components>
    void ES<Components...>::debug() const {
        std::cout << std::endl;
        std::cout << std::endl;
        std::cout << "System " << typeid(ES<Components...>).hash_code() << ":" << std::endl;
        std::cout << "========================" << std::endl;
        std::cout << "Entities: " << entities.size() << std::endl;
        std::cout << "Systems: " << systems.size() << std::endl;
        if (!entities_by_hash.empty()) {
            std::cout << std::endl << "Cached entities by hash:" << std::endl;
            for (auto& x : entities_by_hash)
                std::cout << std::hex << x.first << ": " << x.second.size() << std::endl;
        }
    }

template<typename... Components>
template<typename... Args>
    auto ES<Components...>::_add() {
        static_assert(sizeof...(Args), "Can't instansiate an entity with no components");
        using EC = typename EntityHelper<Components...>::template EntityConstructor<Args...>;
        Entity* e = new EC();
        auto hash = hashf(e->mask);
        entities[e->id] = e;
        for (auto& ecache : entities_by_hash)
            if (hash == ecache.first)
                ecache.second.push_back(e);
        return e;
    }

template<typename... Components>
template<typename... Args, typename F>
    size_t ES<Components...>::add(F f) {
        auto entity = _add<Args...>();
        to<Args...>(entity, f);
        return entity->id;
    }

template<typename... Components>
template<typename... Args>
    size_t ES<Components...>::add() {
        return _add<Args...>()->id;
    }

template<typename... Components>
    void ES<Components...>::remove(size_t id) {
        auto it = entities.find(id);
        if (it != entities.end()) {
            for (auto& ecache : entities_by_hash) {
                auto sit = std::lower_bound(ecache.second.begin(), ecache.second.end(), it->second);
                if (*sit == it->second)
                    ecache.second.erase(sit);
            }
            entities.erase(it);
            delete it->second;
        }
    }

template<typename... Components>
    void ES<Components...>::each(std::function<void(size_t)> f) {
        for (auto& e : entities)
            f(e.first);
    }

template<typename... Components>
    size_t ES<Components...>::size() const {
        return entities.size();
    }

template<typename... Components>
    struct ES<Components...>::System {
        friend class ES<Components...>;
        virtual void update() {}
        virtual void configure() {}
        virtual ~System() {}

    protected:
        template<typename... OtherArgs, typename F>
            void entities(F f) {
                god->each<OtherArgs...>(f);
            }

        template<typename T>
            void subscribe(std::function<void(const T&)>&& f) {
                assert(god);
                god->em.template subscribe<T>(this, std::move(f));
            }

        template<typename T>
            void unsubscribe() {
                assert(god);
                god->em.template unsubscribe<T>(this);
            }

        void unsubscribe() {
            assert(god);
            god->em.unsubscribe(this);
        }

        template<typename T>
            void emit(T event) {
                assert(god);
                god->em.emit(std::move(event));
            }

    private:
        ES<Components...>* god;
    };

} // namespace otus
