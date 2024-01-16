// SPDX-FileCopyrightText: 2023 Limor Fried for Adafruit Industries
//
// SPDX-License-Identifier: MIT

#include <Arduino_GFX_Library.h>
#include "Adafruit_FT6206.h"
#include "Arduino.h"

Arduino_XCA9554SWSPI *expander = new Arduino_XCA9554SWSPI(
    PCA_TFT_RESET, PCA_TFT_CS, PCA_TFT_SCK, PCA_TFT_MOSI,
    &Wire, 0x3F);

Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
    TFT_DE, TFT_VSYNC, TFT_HSYNC, TFT_PCLK,
    TFT_R1, TFT_R2, TFT_R3, TFT_R4, TFT_R5,
    TFT_G0, TFT_G1, TFT_G2, TFT_G3, TFT_G4, TFT_G5,
    TFT_B1, TFT_B2, TFT_B3, TFT_B4, TFT_B5,
    1 /* hync_polarity */, 10 /* hsync_front_porch */, 10 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
    1 /* vsync_polarity */, 15 /* vsync_front_porch */, 2 /* vsync_pulse_width */, 17 /* vsync_back_porch */,
    1 /* pclk_active_neg */);

Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
/* 3.4" square */
//    480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
//    expander, GFX_NOT_DEFINED /* RST */, tl034wvs05_b1477a_init_operations, sizeof(tl034wvs05_b1477a_init_operations));
/* 3.2" bar */
//    320 /* width */, 820 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
//    expander, GFX_NOT_DEFINED /* RST */, tl032fwv01_init_operations, sizeof(tl032fwv01_init_operations));
/* 4.0" 720x720 square */
//    720 /* width */, 720 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
//    expander, GFX_NOT_DEFINED /* RST */, NULL, 0);
/* 4.0" 480x480 square */
//    480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
//    expander, GFX_NOT_DEFINED /* RST */, tl040wvs03_init_operations, sizeof(tl040wvs03_init_operations));
// 2.1" round
    // 480 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */,
    // expander, GFX_NOT_DEFINED /* RST */, TL021WVC02_init_operations, sizeof(TL021WVC02_init_operations));

// HD458002C40 4.58" bar display
//    1 /* hync_polarity */, 10 /* hsync_front_porch */, 10 /* hsync_pulse_width */, 50 /* hsync_back_porch */,
//    1 /* vsync_polarity */, 15 /* vsync_front_porch */, 2 /* vsync_pulse_width */, 17 /* vsync_back_porch */,
//    1 /* pclk_active_neg */, GFX_NOT_DEFINED /* prefer_speed */, false /* useBigEndian */, 0 /* de_idle_high */,
//    0 /* pclk_idle_high */, 80 /* col_offset */, 0 /* row_offset1 */, 8 /* col_offset2 */ );
// static const uint8_t HD458002C40_init_operations[] = {

    320 /* width */, 960 /* height */, rgbpanel, 1 /* rotation */, true /* auto_flush */,
    expander, GFX_NOT_DEFINED /* RST */, HD458002C40_init_operations, sizeof(HD458002C40_init_operations));


Adafruit_FT6206 ctp = Adafruit_FT6206();  // This library also supports FT6336U!

int16_t textHeightBegin()
{
  // return gfx->height();
  return 0;
}

void setup(void)
{
  //while (!Serial) delay(100);

#ifdef GFX_EXTRA_PRE_INIT
  GFX_EXTRA_PRE_INIT();
#endif

  Serial.println("Starting touch paint");

  // Init Display
  Wire.setClock(400000); // speed up I2C
  if (!gfx->begin()) {
    Serial.println("gfx->begin() failed!");
    while (1) yield();
  }

  gfx->fillScreen(BLACK);

  expander->pinMode(PCA_TFT_BACKLIGHT, OUTPUT);
  expander->digitalWrite(PCA_TFT_BACKLIGHT, HIGH);

  Serial.println("Hello!");
  gfx->fillScreen(BLACK);

  // gfx->drawIndexedBitmap
  // gfx->draw16bitRGBBitmap
  // gfx->draw16bitRGBBitmapWithMask
  // gfx->draw16bitRGBBitmapWithTranColor
  // gfx->draw24bitRGBBitmap
  // gfx->draw3bitRGBBitmap
  // gfx->drawBitmap
  // gfx->drawXBitmap

  gfx->setCursor(100, textHeightBegin());
  gfx->setTextSize(5);
  gfx->setTextColor(WHITE);
  gfx->println("Hello World!");

  gfx->setCursor(100, textHeightBegin() + 50);
  gfx->setTextColor(RED);
  gfx->println("RED");

  gfx->setCursor(100, textHeightBegin() + 100);
  gfx->setTextColor(GREEN);
  gfx->println("GREEN");

  gfx->setCursor(100, textHeightBegin() + 150);
  gfx->setTextColor(BLUE);
  gfx->println("BLUE");

  expander->pinMode(PCA_BUTTON_UP, INPUT);
  expander->pinMode(PCA_BUTTON_DOWN, INPUT);
}

void loop()
{
  // use the buttons to turn off
  if (! expander->digitalRead(PCA_BUTTON_DOWN)) {
    expander->digitalWrite(PCA_TFT_BACKLIGHT, LOW);
  }
  // and on the backlight
  if (! expander->digitalRead(PCA_BUTTON_UP)) {
    expander->digitalWrite(PCA_TFT_BACKLIGHT, HIGH);
  }
}
