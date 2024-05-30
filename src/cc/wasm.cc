#include <emscripten/bind.h>

#include "physics/spring.hh"
#include "physics/static.hh"
#include "world/world.hh"

using namespace emscripten;
double static_body_id = 0.;

uintptr_t new_world() {
    auto world = new world::World();
    auto static_body = std::make_unique<physics::StaticBody>(physics::Vector{40, 40});
    auto static_body2 = std::make_unique<physics::StaticBody>(physics::Vector{40, 0});
    static_body->update_velocity(physics::Vector{0, 2});
    static_body2->update_velocity(physics::Vector{0, -2});
    static_body->set_color(graphics::ColorName::ORANGE);
    static_body2->set_color(graphics::ColorName::GREEN);
    world->add_entity(std::move(static_body));
    world->add_entity(std::move(static_body2));

    return (uintptr_t)(world);
}

void tick(uintptr_t world) {
    ((world::World*)world)->tick();
}

uintptr_t get_graphics(uintptr_t world) {
    return (uintptr_t)(((world::World*)world)->get_graphics());
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("new_world", &new_world);
    function("tick", &tick);
    function("get_graphics", &get_graphics);
}