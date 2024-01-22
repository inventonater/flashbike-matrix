#ifndef UTIL_H
#define UTIL_H

#include <cstdint>
#include <cstdlib>
#include <cmath>
#include <random>
#include <limits>

#ifdef ARDUINO
#include <Arduino.h>
#define printy(format, ...) Serial.printf(format, ##__VA_ARGS__)
#else
// #define printy(format, ...) printf(format, ##__VA_ARGS__)
#endif

#define TAG printy("Function: %s, Line: %d\n", __FUNCTION__, __LINE__);
// printf("File: %s, Function: %s, Line: %d\n", __FILE__, __FUNCTION__, __LINE__);
// #define PRINTY(msg) printf("%s, Function: %s, Line: %d\n", msg, __FUNCTION__, __LINE__);
// #define PRINTY(var) printf("Var: %d, Function: %s, Line: %d\n", var, __FUNCTION__, __LINE__);

typedef int16_t color_t;
typedef int16_t hue_t;

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