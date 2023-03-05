#include <Arduino.h>
#include <Wire.h>                 // For I2C communication
#include <Adafruit_Protomatter.h> // For RGB matrix

#define HEIGHT  32 // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH   64 // Matrix width (pixels)
#define MAX_FPS 40 // Maximum redraw rate, frames/second

#if HEIGHT == 64 // 64-pixel tall matrices have 5 address lines:
uint8_t addrPins[] = {17, 18, 19, 20, 21};
#else            // 32-pixel tall matrices have 4 address lines:
uint8_t addrPins[] = {17, 18, 19, 20};
#endif

// Remaining pins are the same for all matrix sizes. These values
// are for MatrixPortal M4. See "simple" example for other boards.
uint8_t rgbPins[] = {7, 8, 9, 10, 11, 12};
uint8_t clockPin = 14;
uint8_t latchPin = 15;
uint8_t oePin = 16;


Adafruit_Protomatter matrix(
        WIDTH, 4, 1, rgbPins, sizeof(addrPins), addrPins,
        clockPin, latchPin, oePin, true);

#define TRAIL_LENGTH 40
#define N_BIKES 4
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
    uint8_t trailIndex;
    uint16_t hue;
};

Bike bikes[N_BIKES];

pos_t spawnPos() {
    pos_t pos;
    pos.x = random(0, WIDTH);
    pos.y = random(0, HEIGHT);
    return pos;
}

// create a random direction for the bike to move in either x or y
// must not be diagonal
pos_t randomDir() {
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

bool shouldBikeTurn(Bike *bike) {
    return random(20) == 0 || isCollision(bike, 10);
}

void turn(Bike *bike) {
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

void moveBike(Bike *bike) {
    pos_t pos = bike->pos[bike->trailIndex];
    pos.x += bike->dir.x;
    pos.y += bike->dir.y;
    bike->trailIndex = (bike->trailIndex + 1) % TRAIL_LENGTH;
    bike->pos[bike->trailIndex] = pos;
}

// colorHSV(uint16_t hue, uint8_t sat = 255, uint8_t val = 255)

uint16_t withBrightnessColor565(uint16_t color, double brightness){
    uint8_t r = (color >> 11) & 0x1F;
    uint8_t g = (color >> 5) & 0x3F;
    uint8_t b = color & 0x1F;
    r = r * brightness;
    g = g * brightness;
    b = b * brightness;
    return matrix.color565(r, g, b);
}

pos_t getTrailIndexPos(Bike *bike, uint8_t trailIndex) {
    return bike->pos[(bike->trailIndex + trailIndex) % TRAIL_LENGTH];
}

void drawBikes() {
    for (int i = 0; i < N_BIKES; i++) {
        Bike *bike = &bikes[i];

        for (int trailIndex = 0; trailIndex < TRAIL_LENGTH; trailIndex++) {
            uint8_t val = (255 * trailIndex / TRAIL_LENGTH);
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
    pBike->pos[0] = spawnPos();
    pBike->dir = randomDir();
    pBike->hue = hue;
}

uint16_t hueForBikeIndex(int i) {
    return (i * 65535 / N_BIKES); //32768
}   

// random hue
uint16_t randomHue() {
    return random(65535);
}

// random hue that is not too close to any other hue
uint16_t randomHueNotTooClose() {
    uint16_t hue = randomHue();
    for (int i = 0; i < N_BIKES; i++) {
        if (abs(hue - bikes[i].hue) < 65535 / N_BIKES / 2) {
            return randomHueNotTooClose();
        }
    }
    return hue;
}

void setup(void) {
    Serial.begin(115200);
    ProtomatterStatus status = matrix.begin();
    Serial.printf("Protomatter begin() status: %d\n", status);

    initColors();
    for (int i = 0; i < N_BIKES; i++) {
        initBike(&bikes[i], hueForBikeIndex(i));
    }
    Serial.printf("%d total bikes\n", N_BIKES);
}

bool shouldWait() {
    static uint32_t prevTime = 0; // Used for frames-per-second throttle

    uint32_t t = micros();
    long delayMicros = 1000000L / MAX_FPS;
    if (t - prevTime < delayMicros) {
        return true;
    }
    prevTime = t;
    return false;
}

void bikeDied(Bike *pBike) {
    Serial.printf("Bike died at %d, %d)\n", pBike->pos[pBike->trailIndex].x, pBike->pos[pBike->trailIndex].y);
    // hueForBikeIndex(pBike - bikes)
    initBike(pBike, randomHue());
}

void loop() {
    // calculations are non-deterministic (don't always take the same amount
    // of time, depending on their current states), this helps ensure that
    // things like gravity appear constant in the simulation.
    if (shouldWait()) return;

    matrix.fillScreen(0x0);
    for (int i = 0; i < N_BIKES; i++) {
        Bike *bike = &bikes[i];
        if (shouldBikeTurn(bike)) {
            turn(bike);
        }
        moveBike(bike);
    }

    for (int i = 0; i < N_BIKES; i++) {
        Bike *bike = &bikes[i];
        if (isCollision(bike, 0)) {
            bikeDied(bike);
        }
    }

    drawBikes();
    matrix.show();
}
