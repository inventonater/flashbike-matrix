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

  bool begin(int32_t speed = GFX_NOT_DEFINED) override
  {
    // matrix.begin();
    fillScreen(0x0);
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
    matrix.show();
  }

private:
  Adafruit_Protomatter matrix = Adafruit_Protomatter(
        WIDTH, bitDepth, 1, rgbPins, sizeof(addrPins), addrPins,
        clockPin, latchPin, oePin, doubleBuffer);
};

Arduino_GFX* gfx = new Protomatter_GFX(WIDTH, HEIGHT);

void renderer_init() {
  Serial.printf("Protomatter...1");
//   while (!Serial) delay(10);
  Serial.printf("Protomatter...2");
  gfx->begin();
}

void renderer_start_frame() {
  // gfx->fillScreen(0x0);
  gfx->startWrite();
}

void renderer_end_frame() {
  gfx->endWrite();
}

void renderer_clear() {
  gfx->fillScreen(0x0);
}

void renderer_draw_pixel(int x, int y, color_t color) {
  gfx->drawPixel(x, y, color);
}

void renderer_draw_rect(int x, int y, int width, int height, uint32_t color) {
  gfx->drawFastHLine(x, y, width, color);
  gfx->drawFastHLine(x, y + height - 1, width, color);
  gfx->drawFastVLine(x, y, height, color);
  gfx->drawFastVLine(x + width - 1, y, height, color);
}

void renderer_draw_filled_rect(int x, int y, int width, int height, uint32_t color) {
  gfx->fillRect(x, y, width, height, color);
}

void renderer_draw_circle(int x, int y, int radius, uint16_t color, uint16_t borderColor) {
  gfx->fillCircle(x, y, radius, color);
  gfx->drawCircle(x, y, radius, borderColor);
}

void renderer_cleanup() {}

color_t renderer_color_hsv(hue_t hue, uint8_t sat, uint8_t val) {
  uint8_t r, g, b;

  hue = (hue * 1530L + 32768) / 65536;

  if (hue < 510) { //         Red to Green-1
    b = 0;
    if (hue < 255) { //         Red to Yellow-1
      r = 255;
      g = hue;       //           g = 0 to 254
    } else {         //         Yellow to Green-1
      r = 510 - hue; //           r = 255 to 1
      g = 255;
    }
  } else if (hue < 1020) { // Green to Blue-1
    r = 0;
    if (hue < 765) { //         Green to Cyan-1
      g = 255;
      b = hue - 510;  //          b = 0 to 254
    } else {          //        Cyan to Blue-1
      g = 1020 - hue; //          g = 255 to 1
      b = 255;
    }
  } else if (hue < 1530) { // Blue to Red-1
    g = 0;
    if (hue < 1275) { //        Blue to Magenta-1
      r = hue - 1020; //          r = 0 to 254
      b = 255;
    } else { //                 Magenta to Red-1
      r = 255;
      b = 1530 - hue; //          b = 255 to 1
    }
  } else { //                 Last 0.5 Red (quicker than % operator)
    r = 255;
    g = b = 0;
  }

  // Apply saturation and value to R,G,B, pack into 16-bit 'RGB565' result:
  uint32_t v1 = 1 + val;  // 1 to 256; allows >>8 instead of /255
  uint16_t s1 = 1 + sat;  // 1 to 256; same reason
  uint8_t s2 = 255 - sat; // 255 to 0
  return (((((r * s1) >> 8) + s2) * v1) & 0xF800) |
         ((((((g * s1) >> 8) + s2) * v1) & 0xFC00) >> 5) |
         (((((b * s1) >> 8) + s2) * v1) >> 11);
}
