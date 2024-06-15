#include <emscripten.h>
#include <emscripten/bind.h>

#include "game/game.hh"

using namespace emscripten;
double static_body_id = 0.;

uintptr_t new_world() {
    auto game = new game::Game(emscripten_get_now());
    return (uintptr_t)(game);
}

void tick(uintptr_t game) {
    auto g = ((game::Game*)game);
    g->world.tick(emscripten_get_now());
}

void setup_spring_center(uintptr_t world, double x, double y) {
    auto g = ((game::Game*)world);
    g->get_p1()->move(x, y);
}

void shoot(uintptr_t world, double x, double y) {
    auto g = ((game::Game*)world);
    g->shoot(x, y);
}

uintptr_t get_graphics(uintptr_t world) {
    return (uintptr_t)(((game::Game*)world)->world.get_graphics());
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("new_world", &new_world);
    function("tick", &tick);
    function("get_graphics", &get_graphics);
    function("setup_spring_center", &setup_spring_center);
    function("shoot", &shoot);
}