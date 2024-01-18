#ifndef GAME_H
#define GAME_H

typedef struct Game {
    // get tick rate
    uint8_t (*get_millis_per_frame)();
    void (*begin)();
    void (*loop)();
} Game;

Game game_create();

#endif // GAME_H
