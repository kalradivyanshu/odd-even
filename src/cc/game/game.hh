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
    bool pickup_exists = false;
    double last_pickup_time = 0.;

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
    }

    void add_bullet(double bullet_id) {
        this->bullet_ids.push_back(bullet_id);

        bool odd = this->bullet_ids.size() % 2 == 1;

        if (odd) {
            this->switch_bullet_state(Team::TEAM_RED);
            this->get_p1()->mark_as_in_danger();
        } else {
            this->switch_bullet_state(Team::TEAM_BLUE);
            this->get_p1()->mark_as_safe();
        }
    }

    void shoot(Player* p, double x, double y) {
        if(p->is_dead()) return;

        auto bullets_count = this->world.control_panel.get_bullets_count(p);
        if(bullets_count == 0) return;

        auto bullet = std::move(p->shoot(x, y));
        auto bullet_id = bullet->get_id();
        bullet->set_color(graphics::WHITE);
        this->world.add_entity(std::move(bullet));
        this->add_bullet(bullet_id);
        this->world.control_panel.decrement_bullets_count(p);

    }

    void shoot_p1(double x, double y) {
        this->shoot(this->get_p1(), x, y);
    }

    void shoot_p2(double x, double y) {
        this->shoot(this->get_p2(), x, y);
    }

    void spawn_pickup() {
        if(this->pickup_exists) return;
        if(this->world.get_time() - this->last_pickup_time < 5000.) return;
        //randomly choose a position
        auto position = physics::Vector(rand() % world::WORLD_SIZE, rand() % world::WORLD_SIZE);
        auto pickup = std::make_unique<Pickup>(position);

        pickup->on_picked_up = [&](physics::Entity* e) {
            if(e->entity_type == physics::EntityType::PLAYER) {
                auto player = dynamic_cast<Player*>(e);
                if(!this->world.control_panel.can_pickup_bullets(player)) return false;
                this->world.control_panel.increment_bullets_count(player);
            }
            this->pickup_exists = false;
            return true;
        };

        this->world.add_entity(std::move(pickup));
        this->pickup_exists = true;
        this->last_pickup_time = this->world.get_time();
    }

    void tick(double time_ms) {
        this->world.tick(time_ms);
        this->spawn_pickup();
    }

    Game(double now) : world(now) {
        auto p1 = std::make_unique<Player>(Team::TEAM_RED);
        this->p1_id = p1->get_id();

        p1->set_position(physics::Vector(10., 40.));
        p1->on_player_hit = [&]() {
            if(this->bullet_state == Team::TEAM_RED) {
                this->get_p1()->mark_as_dead();
                printf("Player 1 is dead\n");
            }
        };


        this->spawn_pickup();

        auto p2 = std::make_unique<Player>(Team::TEAM_BLUE);
        this->p2_id = p2->get_id();

        p2->set_position(physics::Vector(world::WORLD_SIZE - 10., world::WORLD_SIZE - 50.));
        p2->on_player_hit = [&]() {
            if(this->bullet_state == Team::TEAM_BLUE) {
                this->get_p2()->mark_as_dead();
                printf("Player 2 is dead\n");
            }
        };

        this->world.add_entity(std::move(p1));
        this->world.add_entity(std::move(p2));
    }
};

}  // namespace game