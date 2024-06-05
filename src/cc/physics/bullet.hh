#include "physics/entity.hh"

#pragma once

namespace physics {
class Bullet : public Entity {
   public:
    Bullet(Vector position) {
        this->set_position(position);
    }

    void stabilize_velocity() {
        auto vel = this->get_velocity().get_magnitude();
        if (vel < 20) {
            this->velocity.set_magnitude(20);
        }
        if (vel > 20) {
            auto damping_force = this->get_velocity() * -1. * 0.01;
            this->update_force(damping_force);
        } else {
            this->update_force(Vector(0, 0));
        }
    }

    void tick(double elapsed) {
        this->stabilize_velocity();
        this->compute_position(elapsed);
    }
};
}  // namespace physics