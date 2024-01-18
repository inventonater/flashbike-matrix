#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <random>
#include <limits>

typedef int16_t color_t;
typedef int16_t hue_t;

void printy(const char *format, ...);
int sign(const int8_t &x);
uint32_t secToMicros(float s);
uint32_t secToMillis(float s);
uint8_t logmap(uint8_t val);
uint8_t expmap(uint8_t val);
uint32_t remap(uint32_t val, uint32_t p = 3, uint32_t _min = 0, uint32_t _max = 255);
color_t renderer_color_hsv(hue_t hue, uint8_t sat, uint8_t val);
uint16_t color_hueForBikeIndex(int i, int numColors);
uint16_t color_randomHue();
int random(int min, int max);
int random(int max);

#endif //UTIL_H