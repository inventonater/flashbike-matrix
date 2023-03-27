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
