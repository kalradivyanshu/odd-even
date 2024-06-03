#include "physics/entity.hh"

#pragma once

namespace physics {
class SolidBody : public Entity {
   public:
    SolidBody(Vector position) {
        this->set_position(position);
    }

    void tick(double elapsed) {
        this->compute_position(elapsed);
    }
};
}  // namespace physics