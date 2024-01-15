#include <renderer.h>
#include <Arduino.h>
#include <Adafruit_Protomatter.h>

#ifdef ARDUINO_ADAFRUIT_FEATHER_RP2040_SCORPIO
#elif ARDUINO_ADAFRUIT_FEATHER_M4_CAN
#endif

uint8_t rgbPins[] = {
    PIN_PROTOMATTER_RGB_0,
    PIN_PROTOMATTER_RGB_1,
    PIN_PROTOMATTER_RGB_2,
    PIN_PROTOMATTER_RGB_3,
    PIN_PROTOMATTER_RGB_4,
    PIN_PROTOMATTER_RGB_5,
};
uint8_t addrPins[] = {
    PIN_PROTOMATTER_ADDR_0,
    PIN_PROTOMATTER_ADDR_1,
    PIN_PROTOMATTER_ADDR_2,
    PIN_PROTOMATTER_ADDR_3
#ifdef PIN_PROTOMATTER_ADDR_4
    , PIN_PROTOMATTER_ADDR_4
#endif
};

uint8_t clockPin = PIN_PROTOMATTER_CLOCK;
uint8_t latchPin = PIN_PROTOMATTER_LATCH;
uint8_t oePin = PIN_PROTOMATTER_OE;

uint8_t bitDepth = 5;
bool doubleBuffer = true;

#define HEIGHT 32
#define WIDTH 64
#define MAX_FPS 40

Adafruit_Protomatter matrix(
        WIDTH, bitDepth, 1, rgbPins, sizeof(addrPins), addrPins,
        clockPin, latchPin, oePin, doubleBuffer);

void renderer_init() {
  Serial.printf("Protomatter...1");
//   while (!Serial) delay(10);
  Serial.printf("Protomatter...2");
  ProtomatterStatus status = matrix.begin();
  Serial.printf("Protomatter begin() status: %d\n", status);
}

void renderer_clear() {
  matrix.fillScreen(0x0);
}

void renderer_draw_pixel(int x, int y, uint32_t color) {
  matrix.drawPixel(x, y, color);
}

void renderer_draw_rect(int x, int y, int width, int height, uint32_t color) {
  matrix.drawFastHLine(x, y, width, color);
  matrix.drawFastHLine(x, y + height - 1, width, color);
  matrix.drawFastVLine(x, y, height, color);
  matrix.drawFastVLine(x + width - 1, y, height, color);
}

void renderer_draw_filled_rect(int x, int y, int width, int height, uint32_t color) {
  matrix.fillRect(x, y, width, height, color);
}

void renderer_draw_circle(int x, int y, int radius, uint16_t color, uint16_t borderColor) {
  matrix.fillCircle(x, y, radius, color);
  matrix.drawCircle(x, y, radius, borderColor);
}

color_t renderer_color_hsv(uint16_t hue, uint8_t sat, uint8_t val) {
  return matrix.colorHSV(hue, sat, val);
}

void renderer_present() {
  matrix.show();
}

void renderer_cleanup() {}
