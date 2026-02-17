#include <cstdint>
#include "world/color.hh"
#include "world/global.hh"
#include "physics/vector.hh"

#pragma once

namespace game {

class ControlPanel {
   public:
    uint8_t blue_bullets = 5;
    uint8_t red_bullets = 5;

    ControlPanel() {
    }

    void draw_blue_player(uint8_t* graphics) {
        auto position = physics::Vector(5, 2);
        auto left_tail_position = position + physics::Vector(-1, 1);
        auto right_tail_position = position + physics::Vector(1, 1);

        graphics[(int)position.x + (int)position.y * world::WORLD_SIZE] = graphics::BLUE.to_color_u8();
        graphics[(int)left_tail_position.x + (int)left_tail_position.y * world::WORLD_SIZE] = graphics::BLUE.to_color_u8();
        graphics[(int)right_tail_position.x + (int)right_tail_position.y * world::WORLD_SIZE] = graphics::BLUE.to_color_u8();
    }

    void draw_red_player(uint8_t* graphics) {
        auto position = physics::Vector(world::WORLD_SIZE - 5, 2);
        auto left_tail_position = position + physics::Vector(-1, 1);
        auto right_tail_position = position + physics::Vector(1, 1);

        graphics[(int)position.x + (int)position.y * world::WORLD_SIZE] = graphics::RED.to_color_u8();
        graphics[(int)left_tail_position.x + (int)left_tail_position.y * world::WORLD_SIZE] = graphics::RED.to_color_u8();
        graphics[(int)right_tail_position.x + (int)right_tail_position.y * world::WORLD_SIZE] = graphics::RED.to_color_u8();
    }

    void draw(uint8_t* graphics) {
        this->draw_blue_player(graphics);
        this->draw_red_player(graphics);
    }
};

}  // namespace game