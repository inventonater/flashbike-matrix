#ifndef GAME_H
#define GAME_H

typedef struct Game {
    void (*begin)();
    void (*loop)();
} Game;

Game game_create();

#endif // GAME_H
