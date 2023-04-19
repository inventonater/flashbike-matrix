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
};

uint8_t clockPin = PIN_PROTOMATTER_CLOCK; // 13;
uint8_t latchPin = PIN_PROTOMATTER_LATCH; // 0;
uint8_t oePin = PIN_PROTOMATTER_OE // 1;

bool doubleBuffer = true;

#define HEIGHT  32 // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH   64 // Matrix width (pixels)
#define MAX_FPS 40 // Maximum redraw rate, frames/second

Adafruit_Protomatter matrix(
        WIDTH, 4, 1, rgbPins, sizeof(addrPins), addrPins,
        clockPin, latchPin, oePin, doubleBuffer);

static void protomatter_init() {
    matrix.begin();
}

static void protomatter_clear() {
    matrix.fillScreen(0x0);
}

static void protomatter_draw_pixel(int x, int y, uint32_t color) {
    matrix.drawPixel(x, y, color);
}

static void protomatter_draw_rect(int x, int y, int width, int height, uint32_t color) {
    // Draw the top and bottom horizontal lines
    matrix.drawFastHLine(x, y, width, color);
    matrix.drawFastHLine(x, y + height - 1, width, color);

    // Draw the left and right vertical lines
    matrix.drawFastVLine(x, y, height, color);
    matrix.drawFastVLine(x + width - 1, y, height, color);
}

static void protomatter_draw_filled_rect(int x, int y, int width, int height, uint32_t color) {
    matrix.fillRect(x, y, width, height, color);
}

static void protomatter_present() {
    // matrix.swapBuffers(true);
    matrix.show();
}

static void protomatter_cleanup() {
    // matrix.deinit();
}

Renderer renderer_create() {
    Renderer renderer;
    renderer.init = protomatter_init;
    renderer.clear = protomatter_clear;
    renderer.draw_pixel = protomatter_draw_pixel;
    renderer.draw_rect = protomatter_draw_rect;
    renderer.present = protomatter_present;
    renderer.cleanup = protomatter_cleanup;
    return renderer;
}
