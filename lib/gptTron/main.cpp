#include <Arduino.h>
#include "game_logic.h"
#include "display.h"
#include "input.h"

unsigned long lastUpdateTime = 0;
unsigned long updateInterval = 100; // Update every 100ms

void setup() {
  // Initialize game, input controllers, and display
  initGame();
  initInputControllers();
  initDisplay();
}

void loop() {
  unsigned long currentTime = millis();

  if (currentTime - lastUpdateTime >= updateInterval) {
    // Read input
    readInput();

    // Update game state
    updatePlayerDirections();
    updatePlayerPositions();
    checkCollisions();
    updateDisplay();

    // Check for game over condition
    if (isGameOver()) {
      // Handle game over (e.g., display message, reset game, etc.)
    }

    lastUpdateTime = currentTime;
  }
}
