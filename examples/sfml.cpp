#include "../otus.hpp"
#include <SFML/Graphics.hpp>
#include <random>

struct Style {
    float radius;
    sf::Color color;
};

struct Position : public sf::Vector2f {};

using ECS =
    otus::ES<
        Position,
        Style
    >;

struct TremorSystem : public ECS::System {
    void update() override {
        entities<Position>([&](size_t, Position& p) {
            float tx = rand() % 100 - 50,
                  ty = rand() % 100 - 50;
            p.x += tx / 50;
            p.y += ty / 50;
        });
    }
};

struct DrawSystem : public ECS::System {
    sf::RenderWindow& rw;
    DrawSystem(sf::RenderWindow& rw): rw(rw) {}
    void update() override {
        static sf::CircleShape cs;
        entities<Position, Style>([&](size_t, Position& p, Style& s) {
            cs.setFillColor(s.color);
            cs.setRadius(s.radius);
            cs.setPosition(p);
            rw.draw(cs);
        });
    }
};

size_t addEntity(ECS& ecs, float x = 0, float y = 0, float radius = 5) {
    auto handle = ecs.add<Position, Style>();
    ecs.to<Position, Style>(handle, [=](Position& p, Style& s) {
        p.x = x;
        p.y = y;
        s.radius = radius;
        s.color = { 255, 0, 0 };
    });
    return handle;
}

int main() {
    srand(1337);
    sf::RenderWindow window(sf::VideoMode(800, 600), "float");
    ECS ecs;
    ecs.addSystem<TremorSystem>();
    ecs.addSystem<DrawSystem>(window);

    for (std::size_t i = 0; i < 100; ++i) {
        addEntity(ecs, rand() % 800, rand() % 600, rand() % 20 + 5);
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
