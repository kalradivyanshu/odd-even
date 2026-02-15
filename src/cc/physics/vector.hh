#include <cmath>
#include <cstdio>
#include <compare>

#pragma once
namespace physics {

class Vector {
   public:
    double x = 0.;
    double y = 0.;

    Vector operator+(const Vector& other) const {
        return Vector{x + other.x, y + other.y};
    }

    double get_magnitude() const {
        return sqrt(x * x + y * y);
    }

    void set_magnitude(double magnitude) {
        double current_magnitude = get_magnitude();
        x *= magnitude / current_magnitude;
        y *= magnitude / current_magnitude;
    }

    Vector operator-(const Vector& other) const {
        return Vector{x - other.x, y - other.y};
    }

    Vector operator*(double scalar) const {
        return Vector{x * scalar, y * scalar};
    }

    Vector operator/(double scalar) const {
        return Vector{x / scalar, y / scalar};
    }

    Vector operator/(const Vector& other) const {
        return Vector{x / other.x, y / other.y};
    }

    auto operator<=>(const Vector& other) const = default;

    bool compare_discreet(const Vector& other) const {
        return ceil(x) == ceil(other.x) && ceil(y) == ceil(other.y);
    }

    Vector get_normal(const Vector& other) const {
        const auto num = *this - other;
        return num / num.get_magnitude();
    }

    void print() const {
        printf("x: %f\n", x);
        printf("y: %f\n", y);
        printf("\n");
    }

    static Vector random(double max_x = 80., double max_y = 80.) {
        double rx = (double)rand() / (double)RAND_MAX;
        double ry = (double)rand() / (double)RAND_MAX;
        return Vector{max_x * rx, max_y * ry};
    }
};

}  // namespace physics