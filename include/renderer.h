#ifndef RENDERER_H
#define RENDERER_H

#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

typedef struct Renderer {
    void (*init)(void);
    void (*clear)(void);
    void (*draw_pixel)(int x, int y, uint32_t color);
    void (*draw_rect)(int x, int y, int width, int height, uint32_t color);
    void (*present)(void);
    void (*cleanup)(void);
} Renderer;

extern Renderer renderer_create();

typedef struct System {
    uint32_t (*get_ticks)(void);
    void (*delay)(uint32_t ms);
    void (*handle_input_events)(bool *quit, int *dx, int *dy);
} System;

extern System system_create();

#endif // RENDERER_H
