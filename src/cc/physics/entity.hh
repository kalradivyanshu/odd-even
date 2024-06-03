#include <cstdlib>
#include <utility>

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

    void compute_position(double elapsed) {
        this->last_position = this->position;

        this->acceleration = this->force / this->mass;
        this->velocity = this->velocity + this->acceleration * elapsed;

        if (this->velocity.x > 1000) this->velocity.x = 1000;
        if (this->velocity.y > 1000) this->velocity.y = 1000;

        this->position = this->position + this->velocity * elapsed;

        this->bound_collisions();
    }

    void bound_collisions() {
        if (this->position.x < 0) {
            this->position.x = 0;
            this->velocity.x = -this->velocity.x;
        }
        if (this->position.x >= 80) {
            this->position.x = 79;
            this->velocity.x = -this->velocity.x;
        }
        if (this->position.y < 0) {
            this->position.y = 0;
            this->velocity.y = -this->velocity.y;
        }
        if (this->position.y >= 80) {
            this->position.y = 79;
            this->velocity.y = -this->velocity.y;
        }
    }

    void react_to_collision(double m_a, const Vector& v_a_0) {
        auto v_b_0 = this->velocity;
        this->velocity = v_a_0 * (2 * m_a / (this->mass + m_a)) + v_b_0 * (this->mass - m_a) / (this->mass + m_a);
    }

    bool did_collide(Entity* other) {
        bool did_collide = this->position.compare_discreet(other->position);
        if (did_collide) {
            auto v_b_0 = this->velocity;
            this->react_to_collision(other->mass, other->velocity);
            other->react_to_collision(this->mass, v_b_0);
        }
        return did_collide;
    }

    virtual void tick(double elapsed) = 0;
};
}  // namespace physics