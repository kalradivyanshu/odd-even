#include <emscripten/bind.h>

#include "physics/spring.hh"
#include "physics/static.hh"
#include "world/world.hh"

using namespace emscripten;
double static_body_id = 0.;

uintptr_t new_world() {
    auto world = new world::World();

    for (int i = 0; i < 10; i++) {
        auto entity = std::make_unique<physics::StaticBody>(physics::Vector{40, 40});
        entity->set_position(physics::Vector::random());
        entity->update_velocity(physics::Vector::random(3., 3.));
        entity->set_color(graphics::color_from_u8((i + 1) % 4));

        world->add_entity(std::move(entity));
    }

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