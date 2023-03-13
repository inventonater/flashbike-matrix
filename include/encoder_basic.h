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

#define MAX_ENCODERS 5
int seesaw_addrs[] = {0x36, 0x37, 0x38, 0x39, 0x3A};
// int colors[] = {0xff0000, 0x00ff00, 0x0000ff, 0xff00ff, 0xffff00};

struct encoder_t {
    Adafruit_seesaw ss;
    seesaw_NeoPixel sspixel = seesaw_NeoPixel(1, SS_NEOPIX, NEO_GRB + NEO_KHZ800);
    int32_t encoder_position;
};

encoder_t encoders[MAX_ENCODERS];

int num_active_encoders = 0;
int active_encoders[MAX_ENCODERS];

uint32_t Wheel(encoder_t *e, byte WheelPos) {
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

void encoder_setup() {
    for (int i = 0; i < MAX_ENCODERS; i++) {
        // Serial.begin(115200);
// while (!Serial) delay(10);

        Serial.println("Looking for seesaw!");

        if (!(&encoders[i])->ss.begin(seesaw_addrs[i]) || !(&encoders[i])->sspixel.begin(seesaw_addrs[i])) {
            Serial.println("Couldn't find seesaw on default address");
            while (1) delay(10);
        }
        Serial.println("seesaw started");

        uint32_t version = (((&encoders[i])->ss.getVersion() >> 16) & 0xFFFF);
        if (version != 4991) {
            Serial.print("Wrong firmware loaded? ");
            Serial.println(version);
            while (1) delay(10);
        }
        Serial.println("Found Product 4991");

        // set not so bright!
        (&encoders[i])->sspixel.setBrightness(20);
        (&encoders[i])->sspixel.show();

        // use a pin for the built in encoder switch
        (&encoders[i])->ss.pinMode(SS_SWITCH, INPUT_PULLUP);

        // get starting position
        (&encoders[i])->encoder_position = (&encoders[i])->ss.getEncoderPosition();

        Serial.println("Turning on interrupts");
        delay(10);
        (&encoders[i])->ss.setGPIOInterrupts((uint32_t) 1 << SS_SWITCH, 1);
        (&encoders[i])->ss.enableEncoderInterrupt();
    }
}

bool encoder_readPress(encoder_t *e) { return !e->ss.digitalRead(SS_SWITCH); }

int32_t encoder_readPosition(encoder_t *pEncoder) { return pEncoder->ss.getEncoderPosition(); }

bool encoder_isActive(int i) {
    for (int j = 0; j < num_active_encoders; j++) {
        if (active_encoders[j] == i) {
            return true;
        }
    }
    return false;
}


void encoder_loop() {
    for (int i = 0; i < MAX_ENCODERS; i++) {
        encoder_t *pEncoder = &encoders[i];
        bool isPressed = encoder_readPress(pEncoder);

        if (!encoder_isActive(i)) {
            if (isPressed) {
                active_encoders[num_active_encoders++] = i;
            } else {
                continue;
            }
        }

        int32_t newPosition = encoder_readPosition(pEncoder);

        if (pEncoder->encoder_position != newPosition) {
            Serial.printf("Rot: %d %d\n", i, newPosition);

            uint32_t color = Wheel(pEncoder, newPosition & 0xFF);
            pEncoder->sspixel.setPixelColor(0, color);
            pEncoder->sspixel.show();
            pEncoder->encoder_position = newPosition;      // and save for next round
        }
    }
}