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
#include <system.h>

#define SS_SWITCH 24
#define SS_NEOPIX 6

#define N_ENCODERS 4
int seesaw_addrs[] = {0x36, 0x37, 0x38, 0x39, 0x3A};

struct encoder_t {
    bool active;
    Adafruit_seesaw data;
    seesaw_NeoPixel neoPixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
    int32_t position;
    bool isPressed;
};
encoder_t encoders[N_ENCODERS] = {};

bool encoder_isActive(int i) { return i < N_ENCODERS && encoders[i].active; }

bool encoder_readPress(encoder_t *e) { return !e->data.digitalRead(SS_SWITCH); }

int32_t encoder_readPosition(encoder_t *pEncoder) { return pEncoder->data.getEncoderPosition(); }

uint32_t Wheel(encoder_t *e, byte WheelPos) {
    WheelPos = 255 - WheelPos;
    if (WheelPos < 85) {
        return e->neoPixel.Color(255 - WheelPos * 3, 0, WheelPos * 3);
    }
    if (WheelPos < 170) {
        WheelPos -= 85;
        return e->neoPixel.Color(0, WheelPos * 3, 255 - WheelPos * 3);
    }
    WheelPos -= 170;
    return e->neoPixel.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

void encoder_init(uint8_t i) {
    auto *pEncoder = &encoders[i];

    if (!pEncoder->data.begin(seesaw_addrs[i]) || !pEncoder->neoPixel.begin(seesaw_addrs[i])) {
        Serial.printf("Problem starting encoder: %d\n", i);
        return;
    }

    uint32_t version = (pEncoder->data.getVersion() >> 16) & 0xFFFF;
    if (version != 4991) {
        Serial.print("Wrong firmware loaded? ");
        Serial.println(version);
    }

    pEncoder->neoPixel.setBrightness(20);
    pEncoder->neoPixel.show();

    pEncoder->data.pinMode(SS_SWITCH, INPUT_PULLUP);
    pEncoder->position = pEncoder->data.getEncoderPosition();

    // Serial.println("Turning on interrupts");
    // delay(10);
    // pEncoder->ss.setGPIOInterrupts((uint32_t)1 << SS_SWITCH, 1);
    // pEncoder->ss.enableEncoderInterrupt();
}

void encoder_update(uint8_t i) {
    encoder_t *pEncoder = &encoders[i];
    pEncoder->isPressed = encoder_readPress(pEncoder);

    if (!pEncoder->active) {
        if (!pEncoder->isPressed) return;
        pEncoder->active = true;
    }

    int32_t position = encoder_readPosition(pEncoder);

    if (pEncoder->position != position) {
        Serial.printf("Encoder %d rotation: %d\n", i, position);
        uint32_t color = Wheel(pEncoder, position & 0xFF);
        pEncoder->neoPixel.setPixelColor(0, color);
        pEncoder->neoPixel.show();
        pEncoder->position = position;
    }
}

void encoder_initAll() { for (int i = 0; i < N_ENCODERS; i++) encoder_init(i); }

void encoder_updateAll() { for (int i = 0; i < N_ENCODERS; i++) encoder_update(i); }