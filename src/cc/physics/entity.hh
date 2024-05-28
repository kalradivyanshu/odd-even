#include <cstdlib>

#include "physics/vector.hh"
#include "world/color.hh"

#pragma once

namespace physics {

class Entity {
   private:
    Vector position;
    Vector velocity;
    Vector acceleration;
    double mass = 1.0;
    Vector force;
    double id = (double)rand() / (double)RAND_MAX;
    graphics::ColorName color;

   public:
    void update_force(Vector new_force) {
        force = new_force;
    }

    void set_color(graphics::ColorName new_color) {
        color = new_color;
    }

    graphics::ColorName get_color() {
        return color;
    }

    double get_id() {
        return id;
    }

    Vector get_position() {
        return position;
    }

    void set_position(Vector new_position) {
        position = new_position;
    }

    void compute_position() {
        this->acceleration.x = this->force.x / this->mass;
        this->acceleration.y = this->force.y / this->mass;
        this->velocity.x += this->acceleration.x;
        this->velocity.y += this->acceleration.y;
        this->position.x += this->velocity.x;
        this->position.y += this->velocity.y;
    }

    virtual void tick() = 0;
};
}  // namespace physics