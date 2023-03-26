#include <Arduino.h>
#include <Adafruit_Protomatter.h>

uint8_t rgbPins[] = {6, 5, 9, 11, 10, 12};
uint8_t addrPins[] = {A5, A4, A3, A2};
uint8_t clockPin = 13;
uint8_t latchPin = 0;
uint8_t oePin = 1;

#define HEIGHT  32 // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH   64 // Matrix width (pixels)
#define MAX_FPS 40 // Maximum redraw rate, frames/second

Adafruit_Protomatter matrix(
        WIDTH, 4, 1, rgbPins, sizeof(addrPins), addrPins,
        clockPin, latchPin, oePin, true);


static void protomatter_init(void *data) {
    matrix.begin();
}

static void protomatter_clear(void *data) {
    matrix.fillScreen(0x0);
}

static void protomatter_draw_pixel(void *data, int x, int y, uint32_t color) {
    matrix.drawPixel(x, y, color);
}

static void protomatter_draw_rect(void *data, int x, int y, int width, int height, uint32_t color) {
    // Draw the top and bottom horizontal lines
    matrix.drawFastHLine(x, y, width, color);
    matrix.drawFastHLine(x, y + height - 1, width, color);

    // Draw the left and right vertical lines
    matrix.drawFastVLine(x, y, height, color);
    matrix.drawFastVLine(x + width - 1, y, height, color);
}

static void protomatter_draw_filled_rect(void *data, int x, int y, int width, int height, uint32_t color) {
    matrix.fillRect(x, y, width, height, color);
}

static void protomatter_present(void *data) {
    // matrix.swapBuffers(true);
    matrix.show();
}

static void protomatter_cleanup(void *data) {
    // matrix.deinit();
}

Renderer renderer_create() {
    Renderer renderer;
    renderer.init = protomatter_init;
    renderer.clear = protomatter_clear;
    renderer.dr.aw_pixel = protomatter_draw_pixel;
    renderer.draw_rect = protomatter_draw_rect;
    renderer.present = protomatter_present;
    renderer.cleanup = protomatter_cleanup;
    return renderer;
}

uint32_t system_get_ticks(void) {
    return millis();
}

void system_delay(uint32_t ms) {
    delay(ms);
}

System system_create() {
    System system;
    system.get_ticks = system_get_ticks;
    system.delay = system_delay;
    return system;
}

