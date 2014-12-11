#include "../otus.hpp"
#include <chrono>
#include <iostream>

struct A {
    int x;
    int y;
};

struct B {
    int value;
};

struct C {
    float value;
};

struct X1 {};
struct X2 {};
struct X3 {};
struct X4 {};
struct X5 {};
struct X6 {};
struct X7 {};
struct X8 {};
struct X9 {};
struct X10 {};
struct X11 {};
struct X12 {};
struct X13 {};
struct X14 {};
struct X15 {};

using ECS = otus::ES<A, B, C, X1, X2, X3, X4, X5, X6, X7, X8, X9, X10, X11, X12, X13, X14, X15>;

struct OtherSystem : public ECS::System {
    void update() override {
        entities<A>([&](size_t, A&) {
        });
    }
};

struct PrintSystem : public ECS::System {
    void update() override {
        entities<B>([&](size_t, B&) {
        });
    }
};

struct SomeSystem : public ECS::System {
    void update() override {
        entities<C>([&](size_t, C&) {});
    }
};

int main() {
    ECS es;

    es.addSystem<PrintSystem>();
    es.addSystem<OtherSystem>();
    es.addSystem<SomeSystem>();
    es.add<A>();
    es.add<B>();
    es.add<C>();
    es.add<A, C>();

    es.update();
    es.remove(3);
    es.update();
    es.debug();
}
