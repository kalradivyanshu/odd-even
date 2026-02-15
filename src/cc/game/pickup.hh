#include "physics/entity.hh"

#pragma once

namespace game {
class Pickup : public physics::Entity {
    physics::Vector static_position;
    int frame = 0;
    double last_animation_frame_time = 0;
    double current_time = 0;

   public:
    Pickup(physics::Vector position) {
        this->static_position = position;
        this->set_position(position);
        this->entity_type = "Pickup";
        this->on_collision = [this](physics::Entity* e) { this->on_pickup(e); };
        this->radius = 2.0;
        this->mass = 0.0;
    }

    void increment_animation_frame() {
        printf("current_time: %f, last_animation_frame_time: %f\n", this->current_time, this->last_animation_frame_time);
        if((this->current_time - this->last_animation_frame_time) >= 200.) {
            this->frame++;
            this->last_animation_frame_time = this->current_time;
        }
    }

    void draw_animation_first_frame(std::vector<uint8_t>& graphics, const physics::Vector& position) {
        const auto color = graphics::GREEN;
        graphics[(int)position.x + (int)position.y * 80] = color.to_color_u8();
        this->increment_animation_frame();
        return;
    }

    void draw_animation_second_frame(std::vector<uint8_t>& graphics, const physics::Vector& position) {
        const auto color = graphics::GREEN;
        graphics[(int)position.x - 1 + (int)(position.y - 1) * 80] = color.to_color_u8();
        graphics[(int)position.x + 1 + (int)(position.y - 1) * 80] = color.to_color_u8();
        graphics[(int)position.x + 1 + (int)(position.y + 1) * 80] = color.to_color_u8();
        graphics[(int)position.x - 1 + (int)(position.y + 1) * 80] = color.to_color_u8();
        this->increment_animation_frame();
        return;
    }

    void draw_animation_third_frame(std::vector<uint8_t>& graphics, const physics::Vector& position) {
        const auto color = graphics::GREEN;
        graphics[(int)position.x - 1 + (int)(position.y - 1) * 80] = color.to_color_u8();
        graphics[(int)position.x + 1 + (int)(position.y - 1) * 80] = color.to_color_u8();
        graphics[(int)position.x + 1 + (int)(position.y + 1) * 80] = color.to_color_u8();
        graphics[(int)position.x - 1 + (int)(position.y + 1) * 80] = color.to_color_u8();
        graphics[(int)position.x - 2 + (int)(position.y - 2) * 80] = color.to_color_u8();
        graphics[(int)position.x + 2 + (int)(position.y - 2) * 80] = color.to_color_u8();
        graphics[(int)position.x + 2 + (int)(position.y + 2) * 80] = color.to_color_u8();
        graphics[(int)position.x - 2 + (int)(position.y + 2) * 80] = color.to_color_u8();
        this->increment_animation_frame();
        return;
    }

    void draw_animation_fourth_frame(std::vector<uint8_t>& graphics, const physics::Vector& position) {
        const auto color = graphics::GREEN;
        graphics[(int)position.x - 2 + (int)(position.y - 2) * 80] = color.to_color_u8();
        graphics[(int)position.x + 2 + (int)(position.y - 2) * 80] = color.to_color_u8();
        graphics[(int)position.x + 2 + (int)(position.y + 2) * 80] = color.to_color_u8();
        graphics[(int)position.x - 2 + (int)(position.y + 2) * 80] = color.to_color_u8();
        this->increment_animation_frame();
        return;
    }

    void draw_self(std::vector<uint8_t>& graphics) override {
        const auto position = this->get_position();
        const auto color = graphics::GREEN;
        if (position.x < 0 || position.x >= 80 || position.y < 0 || position.y >= 80) {
            return;
        }
        // printf("Pickup Position: (%f, %f)\n", position.x, position.y);
        if(this->frame == 0) {
            graphics[(int)position.x + (int)position.y * 80] = color.to_color_u8();
            graphics[(int)position.x + 1 + (int)position.y * 80] = color.to_color_u8();
            graphics[(int)position.x - 1 + (int)position.y * 80] = color.to_color_u8();

            graphics[(int)position.x + (int)(position.y + 1) * 80] = color.to_color_u8();
            graphics[(int)position.x + (int)(position.y - 1) * 80] = color.to_color_u8();
        }
        if(this->frame == 1) {
            this->draw_animation_first_frame(graphics, position);
        }
        if(this->frame == 2) {
            this->draw_animation_second_frame(graphics, position);
        }
        if(this->frame == 3) {
            this->draw_animation_third_frame(graphics, position);
        }
        if(this->frame == 4) {
            this->draw_animation_fourth_frame(graphics, position);
        }
        return;
    }

    void tick(double elapsed, double time_ms) override {
        this->current_time = time_ms;
        this->compute_position(elapsed);
        this->set_position(this->static_position);
    }

    void on_pickup(physics::Entity* e) {        
        if(this->frame != 0) return;

        printf("Pickup was picked up by %s\n", e->entity_type.c_str());
        this->increment_animation_frame();
    }
};
}  // namespace physics