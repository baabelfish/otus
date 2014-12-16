#pragma once

#include <memory>
#include <unordered_map>
#include <iostream>
#include <functional>

namespace otus {

struct BaseSubscriber {};
template<typename T>
    struct Subscriber : public BaseSubscriber {
        std::function<void(const T&)> f;
    };

class EventManager {
public:
    EventManager() {}
    virtual ~EventManager() {}

    template<typename T>
        void subscribe(void* e, std::function<void(const T&)>&& f) {
            auto p = new Subscriber<T>();
            p->f = std::move(f);
            subscribers[typeid(T).hash_code()][e].reset(std::move(p));
        }

    template<typename T>
        void unsubscribe(void* e) {
            auto it = subscribers.find(typeid(T).hash_code());
            if (it != subscribers.end()) {
                auto eit = it->second.find(e);
                if (eit != it->second.end()) {
                    it->second.erase(eit);
                }
            }
        }

    void unsubscribe(void* e) {
        for (auto& x : subscribers) {
            auto it = x.second.find(e);
            if (it != x.second.end()) {
                x.second.erase(it);
            }
        }
    }

    template<typename T>
        void emit(T event) {
            auto it = subscribers.find(typeid(T).hash_code());
            if (it != subscribers.end()) {
                for (auto& x : it->second) {
                    auto rx = reinterpret_cast<Subscriber<T>*>(x.second.get());
                    rx->f(event);
                }
            }
        }

private:
    std::unordered_map<size_t,
        std::unordered_map<void*, std::unique_ptr<BaseSubscriber>>> subscribers;
};

} // namespace otus
