#include <cstdint>
#include <cstring>
#include <format>
#include <memory>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "physics/entity.hh"
#pragma once

namespace world {
class World {
    std::map<double, std::unique_ptr<physics::Entity>> entities = {};
    std::vector<uint8_t> word_graphics = std::vector<uint8_t>(80 * 80);
    std::unordered_map<std::string, double> collisions = {};
    int time = 0;

   public:
    void add_entity(std::unique_ptr<physics::Entity>&& entity) {
        entities.insert_or_assign(entity->get_id(), std::move(entity));
    }

    std::unique_ptr<physics::Entity>& get_entity(double id) {
        return entities.at(id);
    }

    void remove_entity(double id) {
        entities.erase(id);
    }

    void tick() {
        std::memset(word_graphics.data(), 0, 80 * 80);
        this->did_any_collide();
        for (auto& [id, entity] : entities) {
            entity->tick();
        }

        for (auto& [id, entity] : entities) {
            auto position = entity->get_position();
            auto color = entity->get_color();
            if (position.x < 0 || position.x >= 80 || position.y < 0 || position.y >= 80) {
                continue;
            }
            word_graphics[(int)position.x + (int)position.y * 80] = graphics::color_u8(color);
        }
        this->time++;
    }

    bool did_any_collide() {
        int i = 0;
        bool flag = false;
        for (auto& [id1, entity1] : entities) {
            int j = 0;
            for (auto& [id2, entity2] : entities) {
                if (id1 == id2 || j <= i) {
                    j++;
                    continue;
                }
                std::string collision_key = std::format("{}:{}", id1, id2);
                if (collisions.contains(collision_key) && time - collisions[collision_key] < 2) {
                    continue;
                }
                bool did_collide = entity1->did_collide(entity2.get());
                if (did_collide) {
                    flag = true;
                    collisions[collision_key] = time;
                }
                j++;
            }
            i++;
        }
        return flag;
    }

    uint8_t* get_graphics() {
        return word_graphics.data();
    }
};
}  // namespace world