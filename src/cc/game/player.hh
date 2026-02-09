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
        // we want the player to be origin, and the position to be the cursor
        // we also invert the y axis to match the screen coordinates
        auto radians = std::atan2(-1.*(origin.y - position.y), origin.x - position.x);
        auto angle = radians * 180. / M_PI;
        if(angle < 0) angle += 360.;
        return angle;
    }

    physics::Direction get_direction() const {
        auto angle = this->get_angle();
        if(angle < 22.5 || angle > 360. -22.5) // EAST
            return physics::Direction::EAST;
        if(angle < 67.5 && angle > 22.5) // NORTH-EAST
            return physics::Direction::NORTH_EAST;
        if(angle < 112.5 && angle > 67.5) // NORTH
            return physics::Direction::NORTH;
        if(angle < 157.5 && angle > 112.5) // NORTH-WEST
            return physics::Direction::NORTH_WEST;
        if(angle < 202.5 && angle > 157.5) // WEST
            return physics::Direction::WEST;
        if(angle < 247.5 && angle > 202.5) // SOUTH-WEST
            return physics::Direction::SOUTH_WEST;
        if(angle < 292.5 && angle > 247.5) // SOUTH
            return physics::Direction::SOUTH;
        if(angle < 337.5 && angle > 292.5) // SOUTH-EAST
            return physics::Direction::SOUTH_EAST;
        return physics::Direction::NORTH;
    }

    physics::Vector left_tail_position(const physics::Direction direction, const physics::Vector& position) const {
        switch(direction) {
            case physics::Direction::EAST:       return position + physics::Vector(-1., -1.);
            case physics::Direction::NORTH_EAST: return position + physics::Vector(-1., 0.);
            case physics::Direction::NORTH:      return position + physics::Vector(-1., 1.);
            case physics::Direction::NORTH_WEST: return position + physics::Vector(0., 1.);
            case physics::Direction::WEST:       return position + physics::Vector(1., 1.);
            case physics::Direction::SOUTH_WEST: return position + physics::Vector(1., 0.);
            case physics::Direction::SOUTH:      return position + physics::Vector(1., -1.);
            case physics::Direction::SOUTH_EAST: return position + physics::Vector(0., -1.);
            default:                             return position + physics::Vector(-1., 1.);
        }
    }

    physics::Vector right_tail_position(const physics::Direction direction, const physics::Vector& position) const {
        switch(direction) {
            case physics::Direction::EAST:       return position + physics::Vector(-1., 1.);
            case physics::Direction::NORTH_EAST: return position + physics::Vector(0., 1.);
            case physics::Direction::NORTH:      return position + physics::Vector(1., 1.);
            case physics::Direction::NORTH_WEST: return position + physics::Vector(1., 0.);
            case physics::Direction::WEST:       return position + physics::Vector(1., -1.);
            case physics::Direction::SOUTH_WEST: return position + physics::Vector(0., -1.);
            case physics::Direction::SOUTH:      return position + physics::Vector(-1., -1.);
            case physics::Direction::SOUTH_EAST: return position + physics::Vector(-1., 0.);
            default:                             return position + physics::Vector(1., 1.);
        }
    }

    void draw_self(std::vector<uint8_t>& graphics) override {
        if(this->team != Team::TEAM_RED) return;
        const auto position = this->get_position();
        const auto color = this->get_color();
        const auto direction = this->get_direction();
        const auto left_tail_position = this->left_tail_position(direction, position);
        const auto right_tail_position = this->right_tail_position(direction, position);

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