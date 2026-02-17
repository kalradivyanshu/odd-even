#include "physics/entity.hh"

#pragma once

namespace physics {
class Spring : public Entity {
   private:
    Vector origin;
    double k;
    double damping;
    bool spring_stopped = false;

   protected:
    double current_time = 0.0;
    std::function<void()> stop_spring = []() {};

   public:
    Spring() : Entity() {}
    

    Spring(Vector origin, double k, double damping = 0.0) {
        this->setup(origin, k, damping);
        this->stop_spring = [&]() {
            this->spring_stopped = true;
        };
    }

    Vector get_origin() const {
        return origin;
    }

    void setup(Vector origin, double k, double damping = 0.0) {
        this->origin = origin;
        this->k = k;
        this->damping = damping;
    }

    Vector get_force(Vector position) const {
        Vector displacement = position - origin;
        return displacement * -k;
    }

    void set_origin(Vector new_origin) {
        this->origin = new_origin;
    }

    void tick(double elapsed, double time_ms) override {
        if(this->spring_stopped) {
            this->velocity = physics::Vector(0., 10.);
            this->position = this->position + this->velocity * elapsed;
            return;
        }
        auto force = get_force(get_position());
        auto damping_force = this->get_velocity() * -1. * damping;
        this->update_force(force + damping_force);

        this->compute_position(elapsed);
        this->current_time = time_ms;
    }
};
}  // namespace physics