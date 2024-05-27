namespace physics {
class Vector {
   public:
    double x = 0.;
    double y = 0.;

    Vector operator+(Vector other) {
        return Vector{x + other.x, y + other.y};
    }

    Vector operator-(Vector other) {
        return Vector{x - other.x, y - other.y};
    }

    Vector operator*(double scalar) {
        return Vector{x * scalar, y * scalar};
    }

    void print() {
        printf("x: %f\n", x);
        printf("y: %f\n", y);
        printf("\n");
    }
};
}  // namespace physics