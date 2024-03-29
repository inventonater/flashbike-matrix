#ifndef RENDERER_H
#define RENDERER_H

#include <util.h>

typedef struct RenderContext {
    uint8_t width;
    uint8_t height;
} RenderContext;

void renderer_init(RenderContext context);
void renderer_start_frame(void);
void renderer_end_frame(void);
void renderer_cleanup(void);
void renderer_write_pixel(int x, int y, color_t color);
void renderer_draw_rect(int x, int y, int width, int height, color_t color);
void renderer_draw_filled_rect(int x, int y, int width, int height, color_t color);
void renderer_draw_circle(int x, int y, int radius, color_t color, color_t borderColor);

#endif // RENDERER_H
