#include <cstdint>
namespace graphics {

uint8_t scale_down_to_16bit(uint8_t x) {
    return x >> 4;
}

class Color {
    uint8_t r;
    uint8_t g;
    uint8_t b;

   public:
    Color(uint8_t r, uint8_t g, uint8_t b) {
        this->r = scale_down_to_16bit(r);
        this->g = scale_down_to_16bit(g);
        this->b = scale_down_to_16bit(b);
    }

    uint8_t to_color_u8() const {
        return (r << 4) | (g << 2) | b;
    }

    static Color from_color_u8(uint8_t color) {
        return Color((color & 0b00110000) << 4, (color & 0b00001100) << 2, color & 0b00000011);
    }
};
const Color RED = Color(255, 0, 0);
const Color GREEN = Color(0, 255, 0);
const Color BLUE = Color(0, 0, 255);
const Color ORANGE = Color(255, 165, 0);

}  // namespace graphics