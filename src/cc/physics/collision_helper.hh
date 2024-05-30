#include <algorithm>
#include <utility>

#include "physics/vector.hh"

#pragma once

namespace physics {
int get_sign(const Vector const& loc, const std::pair<Vector, Vector> const& position_update) {
    // two point form of a line
    double a = position_update.second.y - position_update.first.y;
    double b = position_update.first.x - position_update.second.x;

    double y = loc.y - position_update.first.y;
    double x = loc.x - position_update.first.x;

    double result = y / x - a / b;

    if (result > 0) {
        return 1;
    } else if (result < 0) {
        return -1;
    } else {
        return 0;
    }
}

bool static_collision(const Vector const& a, const Vector const& b) {
    return a == b;
}

bool is_static(const std::pair<Vector, Vector> const& update) {
    return update.first == update.second;
}

bool static_moving_collision(const Vector const& static_position, const std::pair<Vector, Vector> const& position_update) {
    if (get_sign(static_position, position_update) != 0) {
        return false;
    }

    if (static_position.x < std::min(position_update.first.x, position_update.second.x) ||
        static_position.x > std::max(position_update.first.x, position_update.second.x)) {
        return false;
    }

    if (static_position.y < std::min(position_update.first.y, position_update.second.y) ||
        static_position.y > std::max(position_update.first.y, position_update.second.y)) {
        return false;
    }

    return true;
}

bool is_colliding(const std::pair<Vector, Vector> const& pos_update1, const std::pair<Vector, Vector> const& pos_update2) {
    if (is_static(pos_update1) && is_static(pos_update2)) {
        return static_collision(pos_update1.first, pos_update2.first);
    }

    if (is_static(pos_update1) && !is_static(pos_update2)) {
        return static_moving_collision(pos_update1.first, pos_update2);
    }

    if (!is_static(pos_update1) && is_static(pos_update2)) {
        return static_moving_collision(pos_update2.first, pos_update1);
    }

    auto sign1 = get_sign(pos_update1.first, pos_update2);
    auto sign2 = get_sign(pos_update1.second, pos_update2);

    if (sign1 == 0 && sign2 == 0) {
        // moving along the same line
        return static_moving_collision(pos_update1.first, pos_update2) ||
               static_moving_collision(pos_update1.second, pos_update2);
    }

    if (sign1 == sign2 && sign1 != 0) {
        return false;
    }

    return true;
}
};  // namespace physics