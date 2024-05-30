namespace graphics {
enum ColorName {
    BLACK = 0,
    GRAY = 1,
    GREEN = 2,
    ORANGE = 3
};

uint8_t color_u8(graphics::ColorName color) {
    switch (color) {
        case graphics::ColorName::BLACK:
            return 0;
        case graphics::ColorName::GRAY:
            return 1;
        case graphics::ColorName::GREEN:
            return 2;
        case graphics::ColorName::ORANGE:
            return 3;
    }
}
}