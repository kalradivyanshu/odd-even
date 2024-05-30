#pragma once
namespace physics {
class Vector {
   public:
    double x = 0.;
    double y = 0.;

    Vector operator+(const Vector& other) const {
        return Vector{x + other.x, y + other.y};
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

    void print() {
        printf("x: %f\n", x);
        printf("y: %f\n", y);
        printf("\n");
    }
};
}  // namespace physics