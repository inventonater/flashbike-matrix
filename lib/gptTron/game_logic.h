#ifndef GAME_LOGIC_H
#define GAME_LOGIC_H

#include <stdint.h>

#define NUMBER_OF_PLAYERS 4
#define GRID_WIDTH 64
#define GRID_HEIGHT 32
extern uint8_t grid[GRID_WIDTH][GRID_HEIGHT]; // Add this line to declare the grid variable

typedef struct {
  int16_t x;
  int16_t y;
  int16_t direction;
  bool alive;
} Player;
extern Player players[NUMBER_OF_PLAYERS];

typedef enum {
  UP = 0,
  RIGHT,
  DOWN,
  LEFT
} Direction;

void initGame();
void updatePlayerDirections();
void updatePlayerPositions();
void checkCollisions();
bool isGameOver();

#endif
