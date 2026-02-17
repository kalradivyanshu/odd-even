#include <cstddef>
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
#include "game/control_panel.hh"

#pragma once

namespace world {
const std::size_t WORLD_GRAPHICS_SIZE = (WORLD_SIZE + TOP_INFO_BAR_HEIGHT) * WORLD_SIZE;
class World {
    std::map<double, std::unique_ptr<physics::Entity>> entities = {};
    std::vector<uint8_t> world_graphics = std::vector<uint8_t>(WORLD_GRAPHICS_SIZE);
    std::vector<uint8_t> compressed_graphics = std::vector<uint8_t>(WORLD_GRAPHICS_SIZE);
    double last_tick_time = 0;
    double average_fps = 0;

   public:
    game::ControlPanel control_panel;
    World(double time_ms) {
        this->last_tick_time = time_ms;
        this->control_panel = game::ControlPanel();
    }

    double get_time() const {
        return this->last_tick_time;
    }

    void add_entity(std::unique_ptr<physics::Entity>&& entity) {
        entities.insert_or_assign(entity->get_id(), std::move(entity));
    }

    void remove_entity(physics::Entity* entity) {
        entities.erase(entity->get_id());
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
        std::memset(world_graphics.data(), 0, WORLD_GRAPHICS_SIZE);

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
            entity->draw_self(world_graphics.data() + (TOP_INFO_BAR_HEIGHT * WORLD_SIZE));
        }

        for (auto it = entities.begin(); it != entities.end(); ) {
            if (it->second->ready_to_be_removed) {
                it = entities.erase(it);
            } else {
                ++it;
            }
        }

        this->control_panel.draw(world_graphics.data());

    }

    bool did_any_collide() {
        int i = 0;
        bool flag = false;
        for (auto& [id1, entity1] : entities) {
            int j = 0;
            for (auto& [id2, entity2] : entities) {
                auto e1 = entity1->entity_type;
                auto e2 = entity2->entity_type;
                if((id1 == id2 || j <= i)
                || (e1 == physics::EntityType::BULLET && e2 == physics::EntityType::BULLET)
                || (e1 == physics::EntityType::PICKUP && e2 == physics::EntityType::BULLET)
                || (e1 == physics::EntityType::BULLET && e2 == physics::EntityType::PICKUP)) {
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