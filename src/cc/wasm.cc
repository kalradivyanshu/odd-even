#include <emscripten/bind.h>

#include "physics/spring.hh"
#include "physics/static.hh"
#include "world/world.hh"

using namespace emscripten;
double static_body_id = 0.;

uintptr_t new_world() {
    auto world = new world::World();
    auto spring = std::unique_ptr<physics::Spring>(new physics::Spring(physics::Vector(40, 40), 0.1));
    auto static_body = std::unique_ptr<physics::StaticBody>(new physics::StaticBody(physics::Vector(40, 40)));
    static_body_id = static_body->get_id();
    static_body->set_color(graphics::ColorName::GRAY);
    spring->set_position(physics::Vector(40, 20));
    spring->set_color(graphics::ColorName::GREEN);
    world->add_entity(std::move(spring));
    world->add_entity(std::move(static_body));

    return (uintptr_t)(world);
}

void tick(uintptr_t world) {
    ((world::World*)world)->tick();
    if (((world::World*)world)->did_any_collide()) {
        printf("Collided\n");
        ((world::World*)world)->get_entity(static_body_id)->set_color(graphics::ColorName::ORANGE);
    } else {
        ((world::World*)world)->get_entity(static_body_id)->set_color(graphics::ColorName::GRAY);
    }
}

uintptr_t get_graphics(uintptr_t world) {
    return (uintptr_t)(((world::World*)world)->get_graphics());
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("new_world", &new_world);
    function("tick", &tick);
    function("get_graphics", &get_graphics);
}