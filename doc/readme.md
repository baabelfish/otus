Documentation
=============

# Classes

## Component
Components are basically just PODs that are used to aggregate entities.

```cpp
struct Position {
  float x;
  float y;
};

struct Radius {
  float value;
};

struct Speed {
  float value;
  float dx;
  float dy;
};

struct Looks {
  Sprite sprite;
};
```

## ECS

```cpp
template<typename... Components> class ECS;
```

* Takes a list of components that can be used with the ECS. You should basically
define a custom class yourself:

```cpp
using MyVeryOwnECS = ECS<Position, Radius, Speed, Looks>;
MyVeryOwnECS ecs;
```


### Methods

#### addSystem
Adds a system into the ECS

```cpp
Renderer r;
ecs.addSystem<GraphicsSystem>(r);
```

#### to
Performs a provided function to an entity. The components are injected into the
the function.

* NOTE: I'll get rid of the template parameters

```cpp
ecs.to(3, [](Position& p, Radius& r) {
    p.x = 42;
    p.y = 1337;
    r.value = 715517;
});
```

#### add
Adds an entity into the ECS. ``O(1)``

```cpp
auto handle = ecs.add<Position, Looks>();
auto handle = ecs.add<Position, Looks>([](Position&, Looks&) {}); // Optional initialization function
```

#### update
Runs all systems. Parameters passed to the update will be passed to the systems.

```cpp
ecs.update();
```

#### debug
Prints the internal state of the ECS.
```cpp
ecs.debug();
```

#### each
Runs a function to all entities with a set of components.

```cpp
ecs.each<Position, Speed>([](size_t id, Position& p, Speed& s) {
    p.x += s.dx * s.value;
    p.y += s.dy * s.value;
});
```

#### remove
Removes an entity from the system ``O(log(n))``.
* NOTE: Will get more efficient in the future

```cpp
ecs.remove(3);
```

#### size
Returns the amount of active entities in the system.



## Systems
Systems are used to define the logic of entities in the ECS.

```cpp
struct GraphicsSystem : public MyVeryOwnECS::System {
    Renderer& renderer;
    GraphicsSystem(Renderer& renderer): renderer(renderer) {}

    void update() override {
        entities<Position, Looks>([&](size_t id, Position& p, Looks& l) {
            std::cout << "Drawing entity: " << id << std::endl;
            renderer.draw(p.x, p.y, l.sprite);
        });
    }
};
```

## Usage
```cpp
using ECS = ECS<Position, Speed>;
ECS ecs;

struct MoveSystem : public MyVeryOwnECS::System {
    void update(double time) override {
        entities<Position, Speed>([&](size_t id, Position& p, Speed& l) {
            p.x += s.dx * s.value * timeywimeystuff(time);
            p.y += s.dy * s.value * timeywimeystuff(time);
        });
    }
};

struct GraphicsSystem : public MyVeryOwnECS::System {
    Renderer& renderer;
    GraphicsSystem(Renderer& renderer): renderer(renderer) {}
    void update() override {
        entities<Position, Looks>([&](size_t id, Position& p, Looks& l) {
            renderer.draw(p.x, p.y, l.sprite);
        });
    }
};

...

size_t createSomething(ECS& ecs, float x, float y) {
    return ecs.add<Position, Speed, Looks>([&](Position& p, Speed& s, Looks& l) {
        p.x = x;
        p.y = y;
        s.dx = 0;
        s.dy = 0;
        s.value = 0;
        l.sprite.loadFromFile("my_cool_sprite.png");
    });
}

int main() {
    ECS ecs;
    double time;
    Renderer r;
    ecs.addSystem<MoveSystem>();
    ecs.addSystem<GraphicsSystem>(r);

    for (size_t i = 0; i < 10; ++i)
        createSomething(ecs, rand() % screenX, rand() % screenY);

    while (true) {
        updateTime(time);
        ecs.update(time);
    }

    return 0;
}

```
