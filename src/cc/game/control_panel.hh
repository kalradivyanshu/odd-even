#include <cstdint>
#include "game/player.hh"
#include "world/color.hh"
#include "world/global.hh"
#include "physics/vector.hh"

#pragma once

namespace game {

class ControlPanel {
   public:
    uint8_t blue_bullets = 3;
    uint8_t red_bullets = 3;
    uint8_t max_bullets = 20;

    ControlPanel() {
    }

    void draw_blue_player(uint8_t* graphics) {
        auto position = physics::Vector(world::WORLD_SIZE - 5, 2);

        auto left_tail_position = position + physics::Vector(-1, 1);
        auto right_tail_position = position + physics::Vector(1, 1);

        graphics[(int)position.x + (int)position.y * world::WORLD_SIZE] = graphics::BLUE.to_color_u8();
        graphics[(int)left_tail_position.x + (int)left_tail_position.y * world::WORLD_SIZE] = graphics::BLUE.to_color_u8();
        graphics[(int)right_tail_position.x + (int)right_tail_position.y * world::WORLD_SIZE] = graphics::BLUE.to_color_u8();
    }

    void draw_red_player(uint8_t* graphics) {
        auto position = physics::Vector(5, 2);
        auto left_tail_position = position + physics::Vector(-1, 1);
        auto right_tail_position = position + physics::Vector(1, 1);

        graphics[(int)position.x + (int)position.y * world::WORLD_SIZE] = graphics::ORANGE.to_color_u8();
        graphics[(int)left_tail_position.x + (int)left_tail_position.y * world::WORLD_SIZE] = graphics::ORANGE.to_color_u8();
        graphics[(int)right_tail_position.x + (int)right_tail_position.y * world::WORLD_SIZE] = graphics::ORANGE.to_color_u8();
    }

    void draw(uint8_t* graphics) {
        this->draw_blue_player(graphics);
        this->draw_red_player(graphics);

        for(int i = 0; i < world::WORLD_SIZE; i++) {
            graphics[i + 6 * world::WORLD_SIZE] = graphics::WHITE.to_color_u8();
        }

        for(int i = 0; i < red_bullets; i++) {
            const auto position = physics::Vector(10 + 2 * i, 3);
            graphics[(int)position.x + (int)position.y * world::WORLD_SIZE] = graphics::WHITE.to_color_u8();
        }

        for(int i = 0; i < blue_bullets; i++) {
            const auto position = physics::Vector(world::WORLD_SIZE - 10 - (2 * i + 1), 3);
            graphics[(int)position.x + (int)position.y * world::WORLD_SIZE] = graphics::WHITE.to_color_u8();
        }
    }

    uint8_t get_bullets_count(game::Player* p) const {
        if(p->team == game::Team::TEAM_RED) {
            return red_bullets;
        } else {
            return blue_bullets;
        }
    }

    bool can_pickup_bullets(game::Player* p) const {
        return this->get_bullets_count(p) < this->max_bullets;
    }

    void increment_bullets_count(game::Player* p) {
        if(p->team == game::Team::TEAM_RED) {
            red_bullets++;
        } else {
            blue_bullets++;
        }
    }

    void decrement_bullets_count(game::Player* p) {
        if(p->team == game::Team::TEAM_RED) {
            red_bullets--;
        } else {
            blue_bullets--;
        }
    }
};

}  // namespace game