namespace graphics {
enum ColorName {
    BLACK = 0,
    GRAY = 1,
    GREEN = 2,
    ORANGE = 3,
    RED = 4,
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
        case graphics::ColorName::RED:
            return 4;
    }
}

graphics::ColorName color_from_u8(uint8_t color) {
    switch (color) {
        case 0:
            return graphics::ColorName::BLACK;
        case 1:
            return graphics::ColorName::GRAY;
        case 2:
            return graphics::ColorName::GREEN;
        case 3:
            return graphics::ColorName::ORANGE;
        case 4:
            return graphics::ColorName::RED;
    }
}
}