#include <renderer.h>
#include <Arduino.h>
#include <Arduino_GFX.h>
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

class Protomatter_GFX : public Arduino_GFX
{
public:
  Protomatter_GFX(int16_t w, int16_t h) : Arduino_GFX(w, h)
  {
    // Initialize Protomatter specific variables here if needed
    ProtomatterStatus status = matrix.begin();
    Serial.printf("Protomatter begin() status: %d\n", status);
  }

  uint16_t colorHSV(uint16_t hue, uint8_t sat, uint8_t val) {
    return matrix.colorHSV(hue, sat, val);
  }

  void clear() {
    matrix.fillScreen(0x0);
  }

  bool begin(int32_t speed = GFX_NOT_DEFINED) override
  {
    clear();
    return true;
  }

  void writePixelPreclipped(int16_t x, int16_t y, uint16_t color) override
  {
    uint8_t r = (color >> 11) & 0x1F;  // Extract the 5-bit red component
    uint8_t g = (color >> 5) & 0x3F;   // Extract the 6-bit green component
    uint8_t b = color & 0x1F;          // Extract the 5-bit blue component

    // Convert the color components to 8-bit values
    r = (r * 527 + 23) >> 6;
    g = (g * 259 + 33) >> 6;
    b = (b * 527 + 23) >> 6;

    color_t typedColor = color;
    renderer_draw_pixel(x, y, typedColor);
  }

  // Implement other necessary methods here
  void endWrite(void) override
  {
    // Apply all batched drawing operations here
  }

  void present() {
    matrix.show();
  }

private:
  Adafruit_Protomatter matrix = Adafruit_Protomatter(
        WIDTH, bitDepth, 1, rgbPins, sizeof(addrPins), addrPins,
        clockPin, latchPin, oePin, doubleBuffer);
};

Protomatter_GFX gfx = Protomatter_GFX(WIDTH, HEIGHT);

void renderer_init() {
  Serial.printf("Protomatter...1");
//   while (!Serial) delay(10);
  Serial.printf("Protomatter...2");
  gfx.begin();
}

void renderer_clear() {
  gfx.clear();
}

void renderer_draw_pixel(int x, int y, color_t color) {
  gfx.drawPixel(x, y, color);
}

void renderer_draw_rect(int x, int y, int width, int height, uint32_t color) {
  gfx.drawFastHLine(x, y, width, color);
  gfx.drawFastHLine(x, y + height - 1, width, color);
  gfx.drawFastVLine(x, y, height, color);
  gfx.drawFastVLine(x + width - 1, y, height, color);
}

void renderer_draw_filled_rect(int x, int y, int width, int height, uint32_t color) {
  gfx.fillRect(x, y, width, height, color);
}

void renderer_draw_circle(int x, int y, int radius, uint16_t color, uint16_t borderColor) {
  gfx.fillCircle(x, y, radius, color);
  gfx.drawCircle(x, y, radius, borderColor);
}

color_t renderer_color_hsv(hue_t hue, uint8_t sat, uint8_t val) {
  return gfx.colorHSV(hue, sat, val);
}

void renderer_present() {
  gfx.present();
}

void renderer_cleanup() {}