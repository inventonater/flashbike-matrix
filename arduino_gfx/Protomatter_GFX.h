#include <Arduino_GFX.h>
#include <Adafruit_Protomatter.h>

class Protomatter_GFX : public Arduino_GFX
{
public:
  Protomatter_GFX(int16_t w, int16_t h, uint8_t bit_depth, uint8_t rgb_pins[], uint8_t addr_pins[], uint8_t clock_pin, uint8_t latch_pin, uint8_t oe_pin, bool doublebuffer) 
    : Arduino_GFX(w, h), matrix(w, h, bit_depth, rgb_pins, addr_pins, clock_pin, latch_pin, oe_pin, doublebuffer)
  {
    // Initialize Protomatter specific variables here if needed
  }

  void begin(void) override
  {
    matrix.begin();
  }

  void drawPixel(int16_t x, int16_t y, uint16_t color) override
  {
    uint8_t r = (color >> 11) & 0x1F;  // Extract the 5-bit red component
    uint8_t g = (color >> 5) & 0x3F;   // Extract the 6-bit green component
    uint8_t b = color & 0x1F;          // Extract the 5-bit blue component

    // Convert the color components to 8-bit values
    r = (r * 527 + 23) >> 6;
    g = (g * 259 + 33) >> 6;
    b = (b * 527 + 23) >> 6;

    matrix.drawPixel(x, y, matrix.Color888(r, g, b));
  }

  // Implement other necessary methods here
  void endWrite(void) override
  {
    // Apply all batched drawing operations here
    matrix.show();
  }
};