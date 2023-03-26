#include <Arduino.h>
#include "game_logic.h"
#include "input.h"

uint8_t grid[GRID_WIDTH][GRID_HEIGHT]; // Define the grid variable here
Player players[NUMBER_OF_PLAYERS];

void initPlayers() {
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    switch (i) {
      case 0:
        players[i].x = GRID_WIDTH / 4;
        players[i].y = GRID_HEIGHT / 4;
        players[i].direction = RIGHT;
        break;
      case 1:
        players[i].x = GRID_WIDTH * 3 / 4;
        players[i].y = GRID_HEIGHT / 4;
        players[i].direction = LEFT;
        break;
      case 2:
        players[i].x = GRID_WIDTH * 3 / 4;
        players[i].y = GRID_HEIGHT * 3 / 4;
        players[i].direction = UP;
        break;
      case 3:
        players[i].x = GRID_WIDTH / 4;
        players[i].y = GRID_HEIGHT * 3 / 4;
        players[i].direction = DOWN;
        break;
    }
    players[i].alive = true;
  }
}

void clearGrid() {
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      grid[x][y] = 0;
    }
  }
}

void initGame() {
  initPlayers();
  clearGrid();
}

void updatePlayerDirections() {
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    if (players[i].alive) {
      int newDirection = getPlayerDirection(i);

      if (newDirection != -1 && (newDirection % 2) != (players[i].direction % 2)) {
        players[i].direction = newDirection;
      }
    }
  }
}

void updatePlayerPositions() {
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    if (players[i].alive) {
      switch (players[i].direction) {
        case UP:
          players[i].y--;
          break;
        case RIGHT:
          players[i].x++;
          break;
        case DOWN:
          players[i].y++;
          break;
        case LEFT:
          players[i].x--;
          break;
      }

      if (players[i].x < 0 || players[i].x >= GRID_WIDTH || players[i].y < 0 || players[i].y >= GRID_HEIGHT) {
        players[i].alive = false;
      } else {
        grid[players[i].x][players[i].y] = i + 1;
      }
    }
  }
}

void checkCollisions() {
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    if (players[i].alive) {
      if (grid[players[i].x][players[i].y] != 0) {
        players[i].alive = false;
      }
    }
  }
}

bool isGameOver() {
  int alivePlayers = 0;
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    if (players[i].alive) {
      alivePlayers++;
    }
  }
  return alivePlayers <= 1;
}
