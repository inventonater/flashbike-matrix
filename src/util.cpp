#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <random>
#include <limits>
#include <util.h>

void printy(const char *format, ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
 }

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

uint32_t remap(uint32_t val, uint32_t p, uint32_t _min, uint32_t _max) {
    // Convert to double for precise division and power calculations
    double normalizedVal = static_cast<double>(val - _min) / (_max - _min);
    double poweredVal = pow(normalizedVal, static_cast<double>(p));

    // Convert back to integer, fitting into the range [_min, _max]
    uint32_t result = static_cast<uint32_t>(poweredVal * (_max - _min) + _min);

    // Ensure the result is clamped within the range
    return (result > _max) ? _max : result;
}

color_t renderer_color_hsv(hue_t hue, uint8_t sat, uint8_t val) {
  uint8_t r, g, b;

  hue = (hue * 1530L + 32768) / 65536;

  if (hue < 510) { //         Red to Green-1
    b = 0;
    if (hue < 255) { //         Red to Yellow-1
      r = 255;
      g = hue;       //           g = 0 to 254
    } else {         //         Yellow to Green-1
      r = 510 - hue; //           r = 255 to 1
      g = 255;
    }
  } else if (hue < 1020) { // Green to Blue-1
    r = 0;
    if (hue < 765) { //         Green to Cyan-1
      g = 255;
      b = hue - 510;  //          b = 0 to 254
    } else {          //        Cyan to Blue-1
      g = 1020 - hue; //          g = 255 to 1
      b = 255;
    }
  } else if (hue < 1530) { // Blue to Red-1
    g = 0;
    if (hue < 1275) { //        Blue to Magenta-1
      r = hue - 1020; //          r = 0 to 254
      b = 255;
    } else { //                 Magenta to Red-1
      r = 255;
      b = 1530 - hue; //          b = 255 to 1
    }
  } else { //                 Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 16-bit 'RGB565' result:
  uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
  uint16_t s1 = 1 + sat;  // 1 to 256; same reason
  uint8_t s2 = 255 - sat; // 255 to 0
  return (((((r * s1) >> 8) + s2) * v1) & 0xF800) |
         ((((((g * s1) >> 8) + s2) * v1) & 0xFC00) >> 5) |
         (((((b * s1) >> 8) + s2) * v1) >> 11);
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