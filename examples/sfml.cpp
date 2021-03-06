#include "../otus.hpp"
#include <SFML/Graphics.hpp>
#include <random>

struct Position : public sf::Vector2f {};

struct Weight {
    float value = 0;
};

struct Style {
    float radius;
    sf::Color color;
};

using ECS =
    otus::ES<
        Position,
        Style,
        Weight
    >;

struct Eventti {
    std::string msg;
};

struct PhysicsSystem : public ECS::System {
    void configure() override {
        subscribe<Eventti>([](const Eventti& e) {
            std::cout << e.msg << std::endl;
        });
    }
    void update() override {
        static size_t times = 0;
        ++times;
        if (times == 150) {
            unsubscribe<Eventti>();
        }
        entities([&](size_t, Position& p, Weight& w) {
            p.y += w.value;
            w.value += 0.01;
            p.y = (int)p.y % 600;
        });
    }
};

struct TremorSystem : public ECS::System {
    void update() override {
        entities([&](size_t, Position& p) {
            float tx = rand() % 100 - 50,
                  ty = rand() % 100 - 50;
            p.x += tx / 50;
            p.y += ty / 50;

            if ((int)p.x % 600 == 0) {
                emit(Eventti{ "Jei!" });
            }
        });
    }
};

struct DrawSystem : public ECS::System {
    sf::RenderWindow& rw;
    DrawSystem(sf::RenderWindow& rw): rw(rw) {}
    void update() override {
        static sf::CircleShape cs;
        entities([&](size_t, Position& p, Style& s) {
            cs.setFillColor(s.color);
            cs.setRadius(s.radius);
            cs.setPosition(p);
            rw.draw(cs);
            Emitter::emit(Eventti{std::to_string(p.x) + ":" + std::to_string(p.y)});
        });
    }
};

size_t addEntity(ECS& ecs, float x = 0, float y = 0, float radius = 5) {
    bool has_physics = rand() % 100 < 20;
    auto handle = has_physics
        ? ecs.add<Position, Style, Weight>()
        : ecs.add<Position, Style>();
    ecs.to(handle, [=](Position& p, Style& s) {
        p.x = x;
        p.y = y;
        s.radius = radius;
        s.color = has_physics ? sf::Color{ 255, 255, 255 } : sf::Color{ 255, 0, 0 };
    });
    return handle;
}

int main() {
    srand(1337);
    sf::RenderWindow window(sf::VideoMode(800, 600), "float");
    ECS ecs;
    ecs.addSystem<PhysicsSystem>();
    ecs.addSystem<TremorSystem>();
    ecs.addSystem<DrawSystem>(window);

    for (std::size_t i = 0; i < 200; ++i) {
        addEntity(ecs, rand() % 800, rand() % 600, rand() % 5 + 1);
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();
        window.clear();
        ecs.update();
        window.display();
    }
    return 0;
}
