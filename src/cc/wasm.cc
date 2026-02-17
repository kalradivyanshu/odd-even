#include <emscripten.h>
#include <emscripten/bind.h>

#include "game/game.hh"
#include "world/global.hh"

using namespace emscripten;
double static_body_id = 0.;

uintptr_t new_world() {
    auto game = new game::Game(emscripten_get_now());
    return (uintptr_t)(game);
}

void tick(uintptr_t game) {
    auto g = ((game::Game*)game);
    g->tick(emscripten_get_now());
}

void move_p1(uintptr_t world, double x, double y) {
    auto g = ((game::Game*)world);
    g->get_p1()->move(x, y);
}

void move_p2(uintptr_t world, double x, double y) {
    auto g = ((game::Game*)world);
    g->get_p2()->move(x, y);
}

void shoot_p1(uintptr_t world, double x, double y) {
    auto g = ((game::Game*)world);
    g->shoot_p1(x, y);
}

void shoot_p2(uintptr_t world, double x, double y) {
    auto g = ((game::Game*)world);
    g->shoot_p2(x, y);
}

uintptr_t get_graphics(uintptr_t world) {
    return (uintptr_t)(((game::Game*)world)->world.get_graphics());
}

uintptr_t get_compressed_graphics(uintptr_t world) {
    return (uintptr_t)(((game::Game*)world)->world.get_compressed_graphics());
}

uint32_t compress_graphics(uintptr_t world) {
    return (uint32_t)((game::Game*)world)->world.compress_graphics();
}

void decompress_graphics(uintptr_t world, size_t compressed_size) {
    ((game::Game*)world)->world.decompress_graphics(compressed_size);
}

double get_average_fps(uintptr_t world) {
    auto g = ((game::Game*)world);
    return g->world.get_average_fps();
}

int get_world_size() {
    return world::WORLD_SIZE;
}

int get_top_info_bar_height() {
    return world::TOP_INFO_BAR_HEIGHT;
}

EMSCRIPTEN_BINDINGS(my_module) {
    function("new_world", &new_world);
    function("tick", &tick);
    function("get_graphics", &get_graphics);
    function("move_p1", &move_p1);
    function("move_p2", &move_p2);
    function("shoot_p1", &shoot_p1);
    function("shoot_p2", &shoot_p2);
    function("get_average_fps", &get_average_fps);
    function("get_world_size", &get_world_size);
    function("get_top_info_bar_height", &get_top_info_bar_height);
    function("get_compressed_graphics", &get_compressed_graphics);
    function("compress_graphics", &compress_graphics);
    function("decompress_graphics", &decompress_graphics);
}