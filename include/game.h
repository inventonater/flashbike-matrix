#ifndef GAME_H
#define GAME_H

#include <util.h>
#include <renderer.h>

typedef struct Game {
    // get tick rate
    uint8_t (*get_millis_per_frame)();
    RenderContext (*get_render_context)();
    void (*begin)();
    void (*loop)();
} Game;

Game game_create();

#endif // GAME_H
