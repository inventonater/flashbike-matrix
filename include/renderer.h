#ifndef RENDERER_H
#define RENDERER_H

#include <stdint.h>

typedef int16_t color_t;
typedef int16_t hue_t;

void renderer_init(void);
void renderer_clear(void);
void renderer_draw_pixel(int x, int y, color_t color);
void renderer_draw_rect(int x, int y, int width, int height, color_t color);
void renderer_draw_filled_rect(int x, int y, int width, int height, color_t color);
void renderer_draw_circle(int x, int y, int radius, color_t color, color_t borderColor);
color_t renderer_color_hsv(hue_t hue, uint8_t sat, uint8_t val);
void renderer_present(void);
void renderer_cleanup(void);

#endif // RENDERER_H
