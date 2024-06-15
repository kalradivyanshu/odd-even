#include "game/bullet.hh"
#include "game/team.hh"
#include "physics/spring.hh"
#include "physics/vector.hh"
#include "world/world.hh"

#pragma once
namespace game {

class Player : public physics::Spring {
   public:
    Team team = Team::TEAM_RED;

    Player(Team team) {
        this->team = team;
        if (this->team == Team::TEAM_RED) {
            this->setup(physics::Vector(10., 10.), 10, 1.3);
            this->set_position(physics::Vector(0., 0.));
            this->set_color(graphics::ORANGE);
        } else {
            this->setup(physics::Vector(79., 79.), 10, 1.3);
            this->set_position(physics::Vector(79., 79.));
            this->set_color(graphics::BLUE);
        }
        this->entity_type = "Player";
        this->on_collision = [this](physics::Entity* e) { this->on_bullet_collision(e); };
    }

    void on_bullet_collision(physics::Entity* e) {
        if (e->entity_type == "Bullet") {
            auto bullet = dynamic_cast<physics::Bullet*>(e);
            if (bullet->team != this->team) {
                printf("Player %s was hit by a bullet\n", this->team == Team::TEAM_RED ? "RED" : "BLUE");
            }
        }
    }

    void move(double x, double y) {
        this->set_origin(physics::Vector(x, y));
    }

    std::unique_ptr<physics::Bullet> shoot(double x, double y) {
        auto pos = this->get_position();
        auto arr_pos = physics::Vector(x, y);
        auto diff = arr_pos - pos;
        if (diff.x == 0 && diff.y == 0) {
            diff = physics::Vector(20., 0.);
        }
        auto vel = diff;
        pos.x += 1.;
        pos.y += 1.;

        auto solid = std::make_unique<physics::Bullet>(pos);
        solid->update_velocity(vel);
        solid->set_color(graphics::GREEN);
        solid->update_mass(0.1);

        return solid;
    }
};

}  // namespace game