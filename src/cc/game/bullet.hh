#include "game/team.hh"
#include "physics/solid_body.hh"

#pragma once

namespace physics {
class Bullet : public Entity {
   public:
    game::Team team = game::Team::TEAM_RED;

    Bullet(Vector position) {
        this->set_position(position);
        this->entity_type = "Bullet";
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

    void tick(double elapsed, double time_ms) override {
        this->stabilize_velocity();
        this->compute_position(elapsed);
    }
};
}  // namespace physics