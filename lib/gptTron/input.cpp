#include <Adafruit_seesaw.h>
#include "input.h"
#include "game_logic.h"

#define NUMBER_OF_PLAYERS 4
#define SEESAW_I2C_ADDRESS_BASE 0x36

Adafruit_seesaw seesawControllers[NUMBER_OF_PLAYERS];

void initInputControllers() {
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    seesawControllers[i].begin((uint8_t)(SEESAW_I2C_ADDRESS_BASE + i));
    seesawControllers[i].pinMode(0, INPUT_PULLUP);
    seesawControllers[i].pinMode(1, INPUT_PULLUP);
    seesawControllers[i].pinMode(2, INPUT_PULLUP);
  }
}

int getPlayerDirection(int playerIndex) {
  uint32_t buttonStates = seesawControllers[playerIndex].digitalReadBulk(0x7);

  if (!(buttonStates & (1 << 0))) {
    return UP;
  } else if (!(buttonStates & (1 << 1))) {
    return RIGHT;
  } else if (!(buttonStates & (1 << 2))) {
    return DOWN;
  } else if (!(buttonStates & (1 << 3))) {
    return LEFT;
  } else {
    return players[playerIndex].direction;
  }
}

void handleSpeedBoost(int playerIndex) {
  if (seesawControllers[playerIndex].analogRead(4) > 512) {
    // Apply the speed boost for the player
  }
}

void readInput() {
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    if (players[i].alive) {
      players[i].direction = getPlayerDirection(i);
      handleSpeedBoost(i);
    }
  }
}
