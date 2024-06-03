#include "physics/entity.hh"

#pragma once

namespace physics {
class Spring : public Entity {
   private:
    Vector origin;
    double k;

   public:
    Spring(Vector origin, double k) {
        this->origin = origin;
        this->k = k;
    }

    Vector get_force(Vector position) {
        Vector displacement = position - origin;
        return displacement * -k;
    }

    void tick(double elapsed) {
        update_force(get_force(get_position()));

        this->compute_position(elapsed);
    }
};
}  // namespace physics