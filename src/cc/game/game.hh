#include "game/bullet.hh"
#include "game/player.hh"
#include "world/world.hh"
#include "game/pickup.hh"

#pragma once

namespace game {

class Game {
   public:
    Team bullet_state = Team::TEAM_RED;
    double p1_id = 0.;
    double p2_id = 0.;
    std::vector<double> bullet_ids = {};
    world::World world;

    Player* get_p(double id) {
        auto p1 = this->world.get_entity(id).get();
        return dynamic_cast<Player*>(p1);
    }

    Player* get_p1() {
        return this->get_p(p1_id);
    }

    Player* get_p2() {
        return this->get_p(p2_id);
    }

    void switch_bullet_state(Team new_team) {
        this->bullet_state = new_team;

        auto p1_team = this->get_p1()->team;

        for (auto& id : this->bullet_ids) {
            auto bullet = dynamic_cast<physics::Bullet*>(this->world.get_entity(id).get());
            bullet->team = new_team;

            if (p1_team == new_team) {
                bullet->set_color(graphics::RED);
            } else {
                bullet->set_color(graphics::GREEN);
            }
        }
    }

    void add_bullet(double bullet_id) {
        this->bullet_ids.push_back(bullet_id);

        bool odd = this->bullet_ids.size() % 2 == 1;

        if (odd) {
            this->switch_bullet_state(Team::TEAM_RED);
        } else {
            this->switch_bullet_state(Team::TEAM_BLUE);
        }
    }

    void shoot(double x, double y) {
        auto p = this->get_p1();

        auto bullet = std::move(p->shoot(x, y));

        auto bullet_id = bullet->get_id();

        this->world.add_entity(std::move(bullet));

        this->add_bullet(bullet_id);
    }

    Game(double now) : world(now) {
        auto p1 = std::make_unique<Player>(Team::TEAM_RED);
        this->p1_id = p1->get_id();

        p1->set_position(physics::Vector(10., 40.));
        auto pickup = std::make_unique<Pickup>(physics::Vector(60., 60.));

        this->world.add_entity(std::move(pickup));

        auto p2 = std::make_unique<Player>(Team::TEAM_BLUE);
        this->p2_id = p2->get_id();

        this->world.add_entity(std::move(p1));
        this->world.add_entity(std::move(p2));
    }
};

}  // namespace game