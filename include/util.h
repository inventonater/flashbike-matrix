#include <Arduino.h>

int sign(const int8_t &x) { return x > 0 ? 1 : -1; }

uint32_t secToMicros(float s) {
    return s * 1000000;
}

uint32_t secToMillis(float s) {
    return s * 1000;
}

uint8_t logmap(uint8_t val) {
    return 255 * log(val + 1) / log(256);
}

uint8_t expmap(uint8_t val) {
    return 255 * (exp(val / 255.0) - 1) / (exp(1) - 1);
}

// remap value cubically
uint32_t remap(uint32_t val, uint32_t p = 3, uint32_t _min = 0, double _max = 255) {
    return min((_max - _min) * pow(val / (_max - _min), p) + _min, _max);
}

uint16_t color_hueForBikeIndex(int i, int numColors) {
    return (i * 65535 / numColors); //
}

uint16_t color_randomHue() {
    return random(65535);
}

// random hue that is not too close to any other hue
//uint16_t color_randomHueNotTooClose(int numColors) {
//    uint16_t hue = color_randomHue();
//    for (int i = 0; i < numColors; i++) {
//        if (abs(hue - bikes[i].hue) < 65535 / numColors / 2) {
//            return color_randomHueNotTooClose(numColors);
//        }
//    }
//    return hue;
//}

// need to pass in matrix.color565(r, g, b)
//uint16_t color_withBrightnessColor565(uint16_t color, double brightness) {
//    uint8_t r = (color >> 11) & 0x1F;
//    uint8_t g = (color >> 5) & 0x3F;
//    uint8_t b = color & 0x1F;
//    r = r * brightness;
//    g = g * brightness;
//    b = b * brightness;
//    return matrix->color565(r, g, b);
//}