#include "game/bullet.hh"
#include "game/team.hh"
#include "physics/spring.hh"
#include "physics/vector.hh"
#include <cmath>

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

    double get_angle() const {
        auto position = this->get_position();
        auto origin = this->get_origin();
        auto radians = -1.*std::atan2(origin.y - position.y, origin.x - position.x);
        auto angle = radians * 180. / M_PI;
        if(angle < 0) angle += 360.;
        return angle;
    }

    physics::Vector left_tail_position(const double angle, const physics::Vector& position) const {
        if(angle < 22.5 || angle > 360. -22.5) // EAST
            return position + physics::Vector(-1., -1.);
        if(angle < 67.5 && angle > 22.5) // NORTH-EAST
            return position + physics::Vector(-1., 0.);
        if(angle < 112.5 && angle > 67.5) // NORTH
            return position + physics::Vector(-1., 1.);
        if(angle < 157.5 && angle > 112.5) // NORTH-WEST
            return position + physics::Vector(1., 0.);
        if(angle < 202.5 && angle > 157.5) // WEST
            return position + physics::Vector(1., 1.);
        if(angle < 247.5 && angle > 202.5) // SOUTH-WEST
            return position + physics::Vector(0., -1.);
        if(angle < 292.5 && angle > 247.5) // SOUTH
            return position + physics::Vector(1., -1.);
        if(angle < 337.5 && angle > 292.5) // SOUTH-EAST
            return position + physics::Vector(0, -1.);
        return position + physics::Vector(-1., 1.);
    }

    physics::Vector right_tail_position(const double angle, const physics::Vector& position) const {
        if(angle < 22.5 || angle > 360. -22.5) // EAST
            return position + physics::Vector(-1., 1.);
        if(angle < 67.5 && angle > 22.5) // NORTH-EAST
            return position + physics::Vector(0., 1.);
        if(angle < 112.5 && angle > 67.5) // NORTH
            return position + physics::Vector(1., 1.);
        if(angle < 157.5 && angle > 112.5) // NORTH-WEST
            return position + physics::Vector(0., 1.);
        if(angle < 202.5 && angle > 157.5) // WEST
            return position + physics::Vector(1., -1.);
        if(angle < 247.5 && angle > 202.5) // SOUTH-WEST
            return position + physics::Vector(1., 0.);
        if(angle < 292.5 && angle > 247.5) // SOUTH
            return position + physics::Vector(-1., -1.);
        if(angle < 337.5 && angle > 292.5) // SOUTH-EAST
            return position + physics::Vector(-1., 0.);   
        return position + physics::Vector(1., 1.);
    }

    void draw_self(std::vector<uint8_t>& graphics) override {
        if(this->team != Team::TEAM_RED) return;
        const auto position = this->get_position();
        const auto color = this->get_color();
        const auto angle = this->get_angle();
        const auto left_tail_position = this->left_tail_position(angle, position);
        const auto right_tail_position = this->right_tail_position(angle, position);

        if (position.x < 0 || position.x >= 80 || position.y < 0 || position.y >= 80) {
            return;
        }

        graphics[(int)position.x + (int)position.y * 80] = color.to_color_u8();
        graphics[(int)left_tail_position.x + (int)left_tail_position.y * 80] = color.to_color_u8();
        graphics[(int)right_tail_position.x + (int)right_tail_position.y * 80] = color.to_color_u8();
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