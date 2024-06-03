#include <emscripten.h>
#include <emscripten/bind.h>

#include "physics/spring.hh"
#include "physics/static.hh"
#include "world/world.hh"

using namespace emscripten;
double static_body_id = 0.;

uintptr_t new_world() {
    auto world = new world::World(emscripten_get_now());

    auto spring = std::make_unique<physics::Spring>(physics::Vector(40., 40.), 10, 1.3);
    static_body_id = spring->get_id();

    spring->set_color(graphics::ORANGE);

    world->add_entity(std::move(spring));

    return (uintptr_t)(world);
}

void tick(uintptr_t world) {
    ((world::World*)world)->tick(emscripten_get_now());
}

void setup_spring_center(uintptr_t world, double x, double y) {
    auto w = ((world::World*)world);
    auto e = w->get_entity(static_body_id).get();
    auto spring = dynamic_cast<physics::Spring*>(e);
    spring->set_origin(physics::Vector(x, y));
}

void shoot(uintptr_t world, double x, double y) {
    auto w = ((world::World*)world);
    auto e = w->get_entity(static_body_id).get();
    auto pos = e->get_position();
    auto arr_pos = physics::Vector(x, y);
    auto vel = arr_pos - pos;

    auto solid = std::make_unique<physics::StaticBody>(pos);
    solid->update_velocity(vel);
    solid->set_color(graphics::GREEN);
    solid->update_mass(0.1);

    w->add_entity(std::move(solid));
}

uintptr_t get_graphics(uintptr_t world) {
    return (uintptr_t)(((world::World*)world)->get_graphics());
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("new_world", &new_world);
    function("tick", &tick);
    function("get_graphics", &get_graphics);
    function("setup_spring_center", &setup_spring_center);
    function("shoot", &shoot);
}