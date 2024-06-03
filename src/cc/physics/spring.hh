#include "physics/entity.hh"

#pragma once

namespace physics {
class Spring : public Entity {
   private:
    Vector origin;
    double k;
    double damping;

   public:
    Spring(Vector origin, double k, double damping = 0.0) {
        this->origin = origin;
        this->k = k;
        this->damping = damping;
    }

    Vector get_force(Vector position) {
        Vector displacement = position - origin;
        return displacement * -k;
    }

    void set_origin(Vector new_origin) {
        this->origin = new_origin;
    }

    void tick(double elapsed) {
        auto force = get_force(get_position());
        auto damping_force = this->get_velocity() * -1. * damping;
        this->update_force(force + damping_force);

        this->compute_position(elapsed);
    }
};
}  // namespace physics