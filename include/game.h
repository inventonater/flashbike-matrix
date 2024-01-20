#ifndef GAME_H
#define GAME_H

#include <util.h>

typedef struct Game {
    // get tick rate
    uint8_t (*get_millis_per_frame)();
    uint8_t (*get_grid_width)();
    uint8_t (*get_grid_height)();
    void (*begin)();
    void (*loop)();
} Game;

Game game_create();

#endif // GAME_H
