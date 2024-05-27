#include <cstdint>
#include <cstring>
#include <memory>
#include <unordered_map>
#include <vector>

#include "physics/entity.hh"
#pragma once

namespace world {
class World {
    std::unordered_map<double, std::unique_ptr<physics::Entity>> entities = {};
    std::vector<uint8_t> word_graphics = std::vector<uint8_t>(80 * 80);

   public:
    void add_entity(std::unique_ptr<physics::Entity>&& entity) {
        entities.insert_or_assign(entity->get_id(), std::move(entity));
    }

    void remove_entity(double id) {
        entities.erase(id);
    }

    void tick() {
        std::memset(word_graphics.data(), 0, 80 * 80);
        for (auto& [id, entity] : entities) {
            entity->tick();
        }

        for (auto& [id, entity] : entities) {
            auto position = entity->get_position();
            auto color = entity->get_color();
            if (position.x < 0 || position.x >= 80 || position.y < 0 || position.y >= 80) {
                continue;
            }
            printf("position.x: %f\n", position.x);
            printf("position.y: %f\n", position.y);
            printf("\n");
            word_graphics[(int)position.x + (int)position.y * 80] = (uint8_t)color;
        }
    }

    uint8_t* get_graphics() {
        return word_graphics.data();
    }
};
}  // namespace world