---- game.h ----

#ifndef GAME_H
#define GAME_H

#include <stdlib.h>
#include <time.h>
#include <renderer.h>
#include <system.h>

typedef struct Game {
    uint32_t (*begin)(System system, Renderer renderer);
    uint32_t (*end)(void);
} Game;

extern Game game_create();

#endif // GAME_H

---- system.h ----

#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdlib.h>
#include <time.h>

typedef struct System {
    uint32_t (*get_millis)(void);
    void (*delay)(uint32_t ms);
    void (*handle_input_events)(bool *quit, int *dx, int *dy);
} System;

extern System system_create();

#endif // SYSTEM_H

---- util.h ----

#include <Arduino.h>

uint32_t secToMicros(uint8_t s) {
    return s * 1000000L;
}

uint32_t secToMillis(uint8_t s) {
    return s * 1000L;
}

---- renderer.h ----

#ifndef RENDERER_H
#define RENDERER_H

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

#endif // RENDERER_H
