#include <Arduino.h>
#include <Wire.h>                 
#include <input_encoder.h>
#include <util.h>
#include <halleffect.h>
#include <Adafruit_Protomatter.h>

// #include "game.h"
// #include "system.h"
// #include "renderer.h"
// #include "util.h"

#define HEIGHT  32 // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH   64 // Matrix width (pixels)
#define MAX_FPS 40 // Maximum redraw rate, frames/second

// uint8_t rgbPins[] = {6, 5, 9, 11, 10, 12};
// uint8_t addrPins[] = {A5, A4, A3, A2};
// uint8_t clockPin = 13;
// uint8_t latchPin = 0;
// uint8_t oePin = 1;

uint8_t rgbPins[]  = {7, 8, 9, 10, 11, 12};
uint8_t addrPins[] = {17, 18, 19, 20};
uint8_t clockPin   = 14;
uint8_t latchPin   = 15;
uint8_t oePin      = 16;

uint8_t bitDepth = 5;

Adafruit_Protomatter matrix(WIDTH, bitDepth, 1, rgbPins, sizeof(addrPins), addrPins, clockPin, latchPin, oePin, true);

#define TRAIL_LENGTH 40
#define N_SPOTS 4
#define N_BIKES 2
typedef int16_t coord_t;
typedef int16_t color_t;

#define N_COLORS   8
#define BOX_HEIGHT 8
color_t colors[N_COLORS];

struct pos_t {
    coord_t x;
    coord_t y;
};

struct Bike {
    pos_t pos[TRAIL_LENGTH];
    pos_t dir;
    uint8_t speed;
    uint32_t nextMoveTime;
    uint8_t trailIndex;
    uint16_t hue;
    uint32_t lastEncoderPosition;
};

Bike bikes[N_BIKES];
int num_humans = 0;

struct Spot {
    pos_t pos;
    uint16_t hue;
    uint8_t radius;
    uint32_t phaseMillis;
    uint32_t current;
};

Spot spots[N_SPOTS];

uint32_t prevTime = 0; // Used for frames-per-second throttle

uint16_t withBrightnessColor565(uint16_t color, double brightness) {
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    r = r * brightness;
    g = g * brightness;
    b = b * brightness;
    return matrix.color565(r, g, b);
}

pos_t randomPosition() {
    pos_t pos;
    pos.x = random(0, WIDTH);
    pos.y = random(0, HEIGHT);
    return pos;
}

pos_t randomDirection() {
    pos_t dir;
    if (random(0, 2) == 0) {
        dir.x = 0;
        dir.y = random(0, 2) * 2 - 1;
    } else {
        dir.x = random(0, 2) * 2 - 1;
        dir.y = 0;
    }
    return dir;
}

bool isCollision(Bike *bike, int distance = 0) {
    pos_t pos = bike->pos[bike->trailIndex];

    while (1 + distance--) {
        for (int i = 0; i < N_BIKES; i++) {
            Bike *other = &bikes[i];

            for (int trailIndex = 0; trailIndex < TRAIL_LENGTH; trailIndex++) {
                pos_t trailPos = other->pos[trailIndex];
                if (pos.x == trailPos.x && pos.y == trailPos.y) {
                    if (trailIndex != other->trailIndex) {
                        return true;
                    }
                }
            }
        }

        // check if bike is out of bounds
        if (pos.x < 0 || pos.x >= WIDTH || pos.y < 0 || pos.y >= HEIGHT) {
            return true;
        }

        pos.x += bike->dir.x;
        pos.y += bike->dir.y;
    }
    return false;
}

bool ai_shouldChangeDirection(Bike *bike) {
    return random(20) == 0 || isCollision(bike, 10);
}

void ai_setRandomDirection(Bike *bike) {
    pos_t dir = bike->dir;
    if (dir.x == 0) {
        bike->dir.x = random(0, 2) * 2 - 1;
        bike->dir.y = 0;
        if (isCollision(bike, 1)) {
            bike->dir.x = -bike->dir.x;
        }
    } else {
        bike->dir.x = 0;
        bike->dir.y = random(0, 2) * 2 - 1;
        if (isCollision(bike, 1)) {
            bike->dir.y = -bike->dir.y;
        }
    }
}

void bike_rotateDirection(Bike *bike, int8_t rotation) {
    if (rotation == 0) return;
    rotation = rotation > 0 ? -1 : 1;
    if (bike->dir.x == 0) {
        bike->dir.x = bike->dir.y * rotation;
        bike->dir.y = 0;
    } else {
        bike->dir.y = -bike->dir.x * rotation;
        bike->dir.x = 0;
    }
}

void moveBike(Bike *bike) {
    pos_t pos = bike->pos[bike->trailIndex];
    pos.x += bike->dir.x;
    pos.y += bike->dir.y;
    bike->trailIndex = (bike->trailIndex + 1) % TRAIL_LENGTH;
    bike->pos[bike->trailIndex] = pos;
}


pos_t getTrailIndexPos(Bike *bike, uint8_t trailIndex) {
    return bike->pos[(bike->trailIndex + trailIndex) % TRAIL_LENGTH];
}

// remap value logarithmically
uint8_t logmap(uint8_t val) {
    return 255 * log(val + 1) / log(256);
}

// remap value exponentially
uint8_t expmap(uint8_t val) {
    return 255 * (exp(val / 255.0) - 1) / (exp(1) - 1);
}

// remap value cubically
uint32_t remap(uint32_t val, uint32_t p = 3, uint32_t _min = 0, double _max = 255) {
    return min((_max - _min) * pow(val / (_max - _min), p) + _min, _max);
}

void drawCircle(int x, int y, int radius, uint16_t color, uint16_t borderColor) {
    matrix.fillCircle(x, y, radius, color);
    matrix.drawCircle(x, y, radius, borderColor);
}

void drawBikes() {
    for (int i = 0; i < N_BIKES; i++) {
        Bike *bike = &bikes[i];

        for (int trailIndex = 0; trailIndex < TRAIL_LENGTH; trailIndex++) {
            // draw trail with gamma correction but no pulse effect 
            uint8_t lerp = 255 * trailIndex / TRAIL_LENGTH;
            uint8_t val = remap(lerp, 3, 16, 160);
            color_t c = matrix.colorHSV(bike->hue, 255, val);
            pos_t pos = getTrailIndexPos(bike, trailIndex);
            matrix.drawPixel(pos.x, pos.y, c);
        }
    }
}

void err(int x) {
    uint8_t i;
    pinMode(LED_BUILTIN, OUTPUT);       // Using onboard LED
    for (i = 1;; i++) {                     // Loop forever...
        digitalWrite(LED_BUILTIN, i & 1); // LED on/off blink to alert user
        delay(x);
    }
}

void initColors() {
    colors[0] = matrix.color565(117, 7, 135); // Purple
    colors[1] = matrix.color565(255, 140, 0); // Orange
    colors[2] = matrix.color565(0, 128, 38); // Green
    colors[3] = matrix.color565(64, 64, 64);  // Dark Gray
    colors[4] = matrix.color565(0, 77, 255); // Blue
    colors[5] = matrix.color565(228, 3, 3); // Red
    colors[6] = matrix.color565(120, 79, 23); // Brown
    colors[7] = matrix.color565(255, 237, 0); // Yellow
}

void initBike(Bike *pBike, uint16_t hue) {
    pBike->trailIndex = 0;
    pBike->pos[0] = randomPosition();
    pBike->dir = randomDirection();
    pBike->hue = hue;
    pBike->speed = random(6, 20);
    pBike->nextMoveTime = millis();
}

uint16_t color_hueForBikeIndex(int i, int range) {
    return (i * 65535 / N_BIKES); //32768
}

// random hue
uint16_t color_randomHue() {
    return random(65535);
}

// random hue that is not too close to any other hue
uint16_t color_randomHueNotTooClose() {
    uint16_t hue = color_randomHue();
    for (int i = 0; i < N_BIKES; i++) {
        if (abs(hue - bikes[i].hue) < 65535 / N_BIKES / 2) {
            return color_randomHueNotTooClose();
        }
    }
    return hue;
}

void initSpot(Spot *pSpot) {
    pSpot->pos = randomPosition();
    pSpot->hue = color_randomHue();
    pSpot->radius = random(1, 4);
    pSpot->phaseMillis = secToMicros(random(1, 4));
    pSpot->current = 0;
}

bool shouldWait(uint32_t t) {
    long delayMicros = 1000000L / MAX_FPS;
    if (t - prevTime < delayMicros) return true;
    return false;
}

void bikeDied(Bike *pBike) {
    Serial.printf("Bike died at %d, %d)\n", pBike->pos[pBike->trailIndex].x, pBike->pos[pBike->trailIndex].y);
    // hueForBikeIndex(pBike - bikes)
    initBike(pBike, color_randomHue());
}

void drawSpot(Spot *pSpot) {
//    uint8_t val = remap(pSpot->current, 3, 16, 255);
    // fade in and out with a sine wave
    uint8_t val = 255 * (sin((double) pSpot->current / (double) pSpot->phaseMillis * 2 * PI) + 1) / 2;

    color_t c = matrix.colorHSV(pSpot->hue, 255, val);
    drawCircle(pSpot->pos.x, pSpot->pos.y, pSpot->radius, 0, c);
}

void setup(void) {
    Serial.begin(115200);
    while (!Serial) delay(10);

    Serial.printf("wtf");
    ProtomatterStatus status = matrix.begin();
    Serial.printf("Protomatter begin() status: %d\n", status);

    initColors();
    for (int i = 0; i < N_BIKES; i++) {
        initBike(&bikes[i], color_hueForBikeIndex(i, N_BIKES));
    }

    for (int i = 0; i < N_SPOTS; i++) {
        initSpot(&spots[i]);
    }

    prevTime = micros();
    Serial.printf("%d total bikes\n", N_BIKES);

    encoder_setup();
}

void loop() {
    encoder_loop();

    uint32_t t = micros();
    if (shouldWait(t)) return;
    uint32_t dt = t - prevTime;

    matrix.fillScreen(0x0);
    for (int i = 0; i < N_BIKES; i++) {
        Bike *bike = &bikes[i];

        while (num_humans < num_active_encoders) {
            bike->lastEncoderPosition = encoders[active_encoders[i]].encoder_position;
            num_humans++;
        }

        if (millis() < bike->nextMoveTime) {
            continue;
        }
        bike->nextMoveTime += secToMillis(1) / bike->speed;

        if (i < num_active_encoders) {
            auto newEncoderPosition = encoders[active_encoders[i]].encoder_position;
            auto rotation = newEncoderPosition - bike->lastEncoderPosition;
            bike_rotateDirection(bike, rotation);
            bike->lastEncoderPosition = newEncoderPosition;
        } else {
            if (ai_shouldChangeDirection(bike)) {
                ai_setRandomDirection(bike);
            }
        }

        moveBike(bike);
    }

    for (int i = 0; i < N_BIKES; i++) {
        Bike *bike = &bikes[i];
        if (isCollision(bike, 0)) {
            bikeDied(bike);
        }
    }

    for (int i = 0; i < N_SPOTS; i++) {
        Spot *spot = &spots[i];
        spot->current += dt;
        if (spot->current > spot->phaseMillis) {
            spot->current = 0;
            initSpot(spot);
        }
        drawSpot(spot);
    }

    drawBikes();
    matrix.show();
    prevTime = t;
}
