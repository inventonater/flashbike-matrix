#ifndef RENDERER_H
#define RENDERER_H

#include <stdlib.h>
#include <time.h>

typedef struct Renderer {
    void (*init)(void);
    void (*clear)(void);
    void (*draw_pixel)(int x, int y, uint32_t color);
    void (*draw_rect)(int x, int y, int width, int height, uint32_t color);
    void (*draw_circle)(int x, int y, int radius, uint16_t color, uint16_t borderColor);
    void (*present)(void);
    void (*cleanup)(void);
} Renderer;

extern Renderer renderer_create();

#endif // RENDERER_H
