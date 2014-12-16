#pragma once

#include <iostream>
#include <unordered_set>

template<typename T>
struct Subscriber {
    friend struct Emitter;

    Subscriber() {
        listeners().insert(this);
    }

    ~Subscriber() {
        listeners().erase(this);
    }

    virtual void on(const T&) = 0;

private:
    static auto& listeners() {
        static std::unordered_set<Subscriber<T>*> temp;
        return temp;
    }
};

struct Emitter {
    template<typename T>
        static void emit(T&& t) {
            auto& lrs = Subscriber<T>::listeners();
            for (auto& sc: lrs) {
                sc->on(t);
            }
        }
};
