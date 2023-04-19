#include "display.h"
#include "game_logic.h"
#include <Adafruit_Protomatter.h>

#define HEIGHT  32 // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH   64 // Matrix width (pixels)
#define MAX_FPS 40 // Maximum redraw rate, frames/second

// uint8_t rgbPins[] = {6, 5, 9, 11, 10, 12};
// uint8_t addrPins[] = {A5, A4, A3, A2};
// uint8_t clockPin = 13;
// uint8_t latchPin = 0;
// uint8_t oePin = 1;

// Adafruit_Protomatter matrix(
//         WIDTH, 4, 1, rgbPins, sizeof(addrPins), addrPins,
//         clockPin, latchPin, oePin, true);

uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;

Adafruit_Protomatter matrix(WIDTH, 4, 1, rgbPins, sizeof(addrPins), addrPins, clockPin, latchPin, oePin, true);


void initDisplay() {
  matrix.begin();
  matrix.show();
}

void drawGrid() {
  for (int x = 0; x < GRID_WIDTH; x++) {
    for (int y = 0; y < GRID_HEIGHT; y++) {
      uint8_t player = grid[x][y];
      if (player > 0) {
        uint16_t color = 0;
        switch (player) {
          case 1:
            color = matrix.color565(255, 0, 0);
            break;
          case 2:
            color = matrix.color565(0, 255, 0);
            break;
          case 3:
            color = matrix.color565(0, 0, 255);
            break;
          case 4:
            color = matrix.color565(255, 255, 0);
            break;
        }
        matrix.drawPixel(x, y, color);
      } else {
        matrix.drawPixel(x, y, 0);
      }
    }
  }
}

void updateDisplay() {
  drawGrid();
  matrix.show();
}
