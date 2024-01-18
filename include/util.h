#include <cstdint>
#include <cstdlib>
#include <cmath>

#include <random>
#include <limits>

void printy(const char *format, ...) { }

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

uint32_t remap(uint32_t val, uint32_t p = 3, uint32_t _min = 0, uint32_t _max = 255) {
    // Convert to double for precise division and power calculations
    double normalizedVal = static_cast<double>(val - _min) / (_max - _min);
    double poweredVal = pow(normalizedVal, static_cast<double>(p));

    // Convert back to integer, fitting into the range [_min, _max]
    uint32_t result = static_cast<uint32_t>(poweredVal * (_max - _min) + _min);

    // Ensure the result is clamped within the range
    return (result > _max) ? _max : result;
}

uint16_t color_hueForBikeIndex(int i, int numColors) {
    return (i * 65535 / numColors); //
}

uint16_t color_randomHue() {
    std::random_device rd; // Obtain a seed from the system's random number generator
    std::mt19937 gen(rd()); // Seed the generator

    // Define a distribution that spans the range of uint16_t
    std::uniform_int_distribution<uint16_t> distrib(0, std::numeric_limits<uint16_t>::max());

    // Generate a random uint16_t
    return distrib(gen);
}

int random(int min, int max) {
    static std::random_device rd; // Obtain a random number from hardware
    static std::mt19937 gen(rd()); // Seed the generator

    std::uniform_int_distribution<> distrib(min, max - 1);

    return distrib(gen);
}

int random(int max) {
    static std::random_device rd; // Obtain a random number from hardware
    static std::mt19937 gen(rd()); // Seed the generator

    std::uniform_int_distribution<> distrib(0, max - 1);

    return distrib(gen);
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