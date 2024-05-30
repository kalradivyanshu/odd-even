#include <cstdlib>
#include <utility>

#include "physics/collision_helper.hh"
#include "physics/vector.hh"
#include "world/color.hh"

#pragma once

namespace physics {

class Entity {
   private:
    Vector last_position;
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

    void update_velocity(Vector new_velocity) {
        velocity = new_velocity;
    }

    void update_mass(double new_mass) {
        this->mass = new_mass;
    }

    void set_color(graphics::ColorName new_color) {
        this->color = new_color;
    }

    graphics::ColorName get_color() {
        return this->color;
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

    std::pair<Vector, Vector> get_position_update() {
        return std::make_pair(last_position, position);
    }

    void compute_position() {
        this->last_position.x = this->position.x;
        this->last_position.y = this->position.y;

        this->acceleration.x = this->force.x / this->mass;
        this->acceleration.y = this->force.y / this->mass;
        this->velocity.x += this->acceleration.x;
        this->velocity.y += this->acceleration.y;
        this->position.x += this->velocity.x;
        this->position.y += this->velocity.y;
    }

    bool did_collide(Entity* other) {
        auto this_update = this->get_position_update();
        auto other_update = other->get_position_update();
        return is_colliding(this_update, other_update);
    }

    virtual void tick() = 0;
};
}  // namespace physics