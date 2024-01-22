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
#define printy(format, ...) printf(format, ##__VA_ARGS__)
#endif

#define TAG printy("Function: %s, Line: %d\n", __FUNCTION__, __LINE__);
// printf("File: %s, Function: %s, Line: %d\n", __FILE__, __FUNCTION__, __LINE__);
// #define PRINTY(msg) printf("%s, Function: %s, Line: %d\n", msg, __FUNCTION__, __LINE__);
// #define PRINTY(var) printf("Var: %d, Function: %s, Line: %d\n", var, __FUNCTION__, __LINE__);

typedef uint16_t color_t;
typedef uint32_t c8888_t;
typedef uint16_t hue_t;

int sign(const int8_t &x);
uint32_t secToMicros(float s);
uint32_t secToMillis(float s);
uint8_t logmap(uint8_t val);
uint8_t expmap(uint8_t val);
uint32_t remap(uint32_t val, uint32_t p = 3, uint32_t _min = 0, uint32_t _max = 255);
color_t renderer_color_hsv(hue_t hue, uint8_t sat, uint8_t val);
hue_t color_hueForBikeIndex(int i, int numColors);
hue_t color_randomHue();
int random(int min, int max);
int random(int max);

c8888_t RGB888toRGBA8888(uint8_t r, uint8_t g, uint8_t b, uint8_t a) ;
color_t RGB888toRGB565(uint8_t r, uint8_t g, uint8_t b) ;
color_t RGBA8888toRGB565(c8888_t colorRGBA) ;
void RGBA8888toRGB888(c8888_t colorRGBA, uint8_t *r, uint8_t *g, uint8_t *b) ;
void RGB565toRGB888(color_t color565, uint8_t *r, uint8_t *g, uint8_t *b) ;
c8888_t RGB565toRGBA8888(color_t color565, uint8_t a = 255) ;

#endif //UTIL_H