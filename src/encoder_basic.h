/*
 * This example shows how to read from a seesaw encoder module.
 * The available encoder API is:
 *      int32_t getEncoderPosition();
        int32_t getEncoderDelta();
        void enableEncoderInterrupt();
        void disableEncoderInterrupt();
        void setEncoderPosition(int32_t pos);
 */
#include "Adafruit_seesaw.h"
#include <seesaw_neopixel.h>

#define SS_SWITCH        24
#define SS_NEOPIX        6

#define NUM_ENCODERS 5
int seesaw_addrs[] =  { 0x36, 0x37, 0x38, 0x39, 0x3A };
// int colors[] = {0xff0000, 0x00ff00, 0x0000ff, 0xff00ff, 0xffff00};

struct encoder_t {
  Adafruit_seesaw ss;
  seesaw_NeoPixel sspixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
  int32_t encoder_position;
};
encoder_t encoders[NUM_ENCODERS];

uint32_t Wheel(encoder_t* e, byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if (WheelPos < 85) {
    return e->sspixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if (WheelPos < 170) {
    WheelPos -= 85;
    return e->sspixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return e->sspixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void encoder_setup(encoder_t* e, int addr) {
  // Serial.begin(115200);
  // while (!Serial) delay(10);

  Serial.println("Looking for seesaw!");
  
  if (! e->ss.begin(addr) || ! e->sspixel.begin(addr)) {
    Serial.println("Couldn't find seesaw on default address");
    while(1) delay(10);
  }
  Serial.println("seesaw started");

  uint32_t version = ((e->ss.getVersion() >> 16) & 0xFFFF);
  if (version  != 4991){
    Serial.print("Wrong firmware loaded? ");
    Serial.println(version);
    while(1) delay(10);
  }
  Serial.println("Found Product 4991");

  // set not so bright!
  e->sspixel.setBrightness(20);
  e->sspixel.show();
  
  // use a pin for the built in encoder switch
  e->ss.pinMode(SS_SWITCH, INPUT_PULLUP);

  // get starting position
  e->encoder_position = e->ss.getEncoderPosition();

  Serial.println("Turning on interrupts");
  delay(10);
  e->ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
  e->ss.enableEncoderInterrupt();
}

void encoder_loop(encoder_t* e) {
  if (! e->ss.digitalRead(SS_SWITCH)) {
    Serial.println("Button pressed!");
  }

  int32_t new_position = e->ss.getEncoderPosition();
  // did we move arounde?
  if (e->encoder_position != new_position) {
    Serial.println(new_position);         // display new position

    // change the neopixel color
    e->sspixel.setPixelColor(0, Wheel(e, new_position & 0xFF));
    e->sspixel.show();
    e->encoder_position = new_position;      // and save for next round
  }

  // don't overwhelm serial port
  delay(10);
}

void do_encoder_setup(){
  for (int i = 0; i < NUM_ENCODERS; i++) {
    encoder_setup(&encoders[i], seesaw_addrs[i]);
  }
}

void do_encoder_loop(){
  for (int i = 0; i < NUM_ENCODERS; i++) {
    encoder_loop(&encoders[i]);
  }
}