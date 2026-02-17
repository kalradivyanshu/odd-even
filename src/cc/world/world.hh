#include <cstdint>
#include <cstring>
#include <format>
#include <memory>
#include <string>
#include <map>
#include <utility>
#include <vector>

#include "physics/entity.hh"
#include "world/global.hh"
#include "world/compress.hh"

#pragma once

namespace world {
class World {
    std::map<double, std::unique_ptr<physics::Entity>> entities = {};
    std::vector<uint8_t> world_graphics = std::vector<uint8_t>(WORLD_SIZE * WORLD_SIZE);
    std::vector<uint8_t> compressed_graphics = std::vector<uint8_t>(WORLD_SIZE * WORLD_SIZE);
    double last_tick_time = 0;
    double average_fps = 0;

   public:
    World(double time_ms) {
        this->last_tick_time = time_ms;
    }

    void add_entity(std::unique_ptr<physics::Entity>&& entity) {
        entities.insert_or_assign(entity->get_id(), std::move(entity));
    }

    std::unique_ptr<physics::Entity>& get_entity(double id) {
        return entities.at(id);
    }

    void remove_entity(double id) {
        entities.erase(id);
    }

    double get_average_fps() const {
        return this->average_fps;
    }

    void tick(double time_ms) {
        std::memset(world_graphics.data(), 0, WORLD_SIZE * WORLD_SIZE);

        double average_fps = 1000. / (time_ms - this->last_tick_time);
        this->average_fps = (this->average_fps * 0.9) + (average_fps * 0.1);

        while (this->last_tick_time < time_ms) {
            this->did_any_collide();
            for (auto& [id, entity] : entities) {
                entity->tick(0.001, time_ms);
            }
            this->last_tick_time += 1.;
        }

        for (auto& [id, entity] : entities) {
            entity->draw_self(world_graphics);
        }

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
                if(entity1->entity_type == physics::EntityType::BULLET && entity2->entity_type == physics::EntityType::BULLET) {
                    j++;
                    continue;
                }
                bool did_collide = entity1->did_collide(entity2.get());
                flag |= did_collide;
                j++;
            }
            i++;
        }
        return flag;
    }

    uint8_t* get_graphics() {
        return world_graphics.data();
    }

    uint8_t* get_compressed_graphics() {
        return compressed_graphics.data();
    }

    void decompress_graphics(size_t compressed_size) {
        decompress(compressed_graphics, world_graphics, compressed_size);
    }

    uLongf compress_graphics() {
        return compress(world_graphics, compressed_graphics);
    }
};
}  // namespace world