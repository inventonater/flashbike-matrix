#include <Arduino.h>
#include <Wire.h>
#include "../include/input_encoder.h"
#include "../include/input_wiichuck.h"
#include "../include/util.h"
#include "../include/halleffect.h"
#include "../.pio/libdeps/adafruit_feather_m4_can/Adafruit Protomatter/src/core.h"
#include <Adafruit_Protomatter.h>
#include <WiiChuck.h>

#define HEIGHT 32  // Matrix height (pixels) - SET TO 64 FOR 64x64 MATRIX!
#define WIDTH 64   // Matrix width (pixels)
#define MAX_FPS 40 // Maximum redraw rate, frames/second

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
    PIN_PROTOMATTER_ADDR_3};

uint8_t clockPin = PIN_PROTOMATTER_CLOCK; // 13;
uint8_t latchPin = PIN_PROTOMATTER_LATCH; // 0;
uint8_t oePin = PIN_PROTOMATTER_OE;       // 1;
uint8_t bitDepth = 5;

Adafruit_Protomatter matrix(WIDTH, bitDepth, 1, rgbPins, sizeof(addrPins), addrPins, clockPin, latchPin, oePin, true);

#define RESPAWN_TIME 4
#define TRAIL_LENGTH 80
#define N_SPOTS 0
#define N_BIKES 4

typedef int16_t coord_t;
typedef int16_t color_t;

struct pos_t
{
    coord_t x;
    coord_t y;
};

struct Bike
{
    pos_t pos[TRAIL_LENGTH];
    pos_t dir;
    uint8_t speed;
    int lives;
    uint32_t nextMoveTime;
    uint32_t diedTime;
    uint8_t trailIndex;
    uint16_t hue;
    uint32_t lastEncoderPosition;
};

Bike bikes[N_BIKES];

struct Spot
{
    pos_t pos;
    uint16_t hue;
    uint8_t radius;
    uint32_t phaseMillis;
    uint32_t current;
};

Spot spots[N_SPOTS];

uint32_t prevTimeMillis = 0; // Used for frames-per-second throttle

pos_t randomPosition()
{
    pos_t pos;
    pos.x = random(0, WIDTH);
    pos.y = random(0, HEIGHT);
    return pos;
}

pos_t randomDirection()
{
    pos_t dir;
    if (random(0, 2) == 0)
    {
        dir.x = 0;
        dir.y = random(0, 2) * 2 - 1;
    }
    else
    {
        dir.x = random(0, 2) * 2 - 1;
        dir.y = 0;
    }
    return dir;
}

bool isCollision(Bike *bike, int distance = 0)
{
    pos_t pos = bike->pos[bike->trailIndex];

    while (1 + distance--)
    {
        for (int i = 0; i < N_BIKES; i++)
        {
            Bike *other = &bikes[i];

            for (int trailIndex = 0; trailIndex < TRAIL_LENGTH; trailIndex++)
            {
                pos_t trailPos = other->pos[trailIndex];
                if (pos.x == trailPos.x && pos.y == trailPos.y)
                {
                    if (trailIndex != other->trailIndex)
                    {
                        return true;
                    }
                }
            }
        }

        if (pos.x < 0 || pos.x >= WIDTH || pos.y < 0 || pos.y >= HEIGHT)
        {
            return true;
        }

        pos.x += bike->dir.x;
        pos.y += bike->dir.y;
    }
    return false;
}

bool ai_shouldChangeDirection(Bike *bike)
{
    return random(20) == 0 || isCollision(bike, 10);
}

void ai_setRandomDirection(Bike *bike)
{
    pos_t dir = bike->dir;
    if (dir.x == 0)
    {
        bike->dir.x = random(0, 2) * 2 - 1;
        bike->dir.y = 0;
        if (isCollision(bike, 1))
        {
            bike->dir.x = -bike->dir.x;
        }
    }
    else
    {
        bike->dir.x = 0;
        bike->dir.y = random(0, 2) * 2 - 1;
        if (isCollision(bike, 1))
        {
            bike->dir.y = -bike->dir.y;
        }
    }
}

bool dir_isOpposite(pos_t dir1, pos_t dir2)
{
    if (dir1.x == dir2.x && dir1.y == -dir2.y)
        return true;
    if (dir1.x == -dir2.x && dir1.y == dir2.y)
        return true;
    return false;
}

void bike_setJoyDirection(int8_t player_index, Bike *bike, int8_t x, int8_t y)
{
    if (bike->lives < 1)
    {
        bike->dir = {0, 0};
        bike->speed = 0;
        return;
    }

    pos_t dir = {-x, y};
    if (dir.x == 0 && dir.y == 0)
        return;

    int8_t tmpX = dir.x;
    switch (player_index)
    {
    case 1: // Rotate 90 degrees
        dir.x = dir.y;
        dir.y = -tmpX;
        break;
    case 2: // Rotate 180 degrees
        dir.x = -dir.x;
        dir.y = -dir.y;
        break;
    case 3: // Rotate 270 degrees (or -90 degrees)
        dir.x = -dir.y;
        dir.y = tmpX;
        break;
    default:
        break;
    }

    if (bike->dir.x != dir.x || bike->dir.y != dir.y)
    {
        // Serial.printf("Joy %d: %d %d\n", player_index, dir.x, dir.y);
        if (!dir_isOpposite(dir, bike->dir))
            bike->dir = dir;
    }
}

void bike_rotateDirection(Bike *bike, int8_t rotation)
{
    if (rotation == 0)
        return;
    rotation = rotation > 0 ? -1 : 1;
    if (bike->dir.x == 0)
    {
        bike->dir.x = bike->dir.y * rotation;
        bike->dir.y = 0;
    }
    else
    {
        bike->dir.y = -bike->dir.x * rotation;
        bike->dir.x = 0;
    }
}

void moveBike(Bike *bike)
{
    pos_t pos = bike->pos[bike->trailIndex];
    pos.x += bike->dir.x;
    pos.y += bike->dir.y;
    bike->trailIndex = (bike->trailIndex + 1) % TRAIL_LENGTH;
    bike->pos[bike->trailIndex] = pos;
}

pos_t getTrailIndexPos(Bike *bike, uint8_t trailIndex)
{
    return bike->pos[(bike->trailIndex + trailIndex) % TRAIL_LENGTH];
}

void drawBikes()
{
    for (int i = 0; i < N_BIKES; i++)
    {
        Bike *bike = &bikes[i];

        for (int trailIndex = 0; trailIndex < TRAIL_LENGTH; trailIndex++)
        {
            // draw trail with gamma correction but no pulse effect
            uint8_t lerp = 255 * trailIndex / TRAIL_LENGTH;
            uint8_t val = remap(lerp, 3, 16, 160);
            color_t c = matrix.colorHSV(bike->hue, 255, val);
            pos_t pos = getTrailIndexPos(bike, trailIndex);
            matrix.drawPixel(pos.x, pos.y, c);
        }
    }
}

void err(int x)
{
    uint8_t i;
    pinMode(LED_BUILTIN, OUTPUT); // Using onboard LED
    for (i = 1;; i++)
    {                                     // Loop forever...
        digitalWrite(LED_BUILTIN, i & 1); // LED on/off blink to alert user
        delay(x);
    }
}

void initBike(Bike *pBike, uint16_t hue)
{
    for (int i = 0; i < TRAIL_LENGTH; i++)
    {
        pBike->pos[i] = (pos_t){0, 0};
    }
    pBike->trailIndex = 0;
    pBike->pos[0] = randomPosition();
    pBike->hue = hue;
    pBike->nextMoveTime = millis();
    pBike->diedTime = 0;
}

void initSpot(Spot *pSpot)
{
    pSpot->pos = randomPosition();
    pSpot->hue = color_randomHue();
    pSpot->radius = random(1, 4);
    pSpot->phaseMillis = secToMillis(random(1, 4));
    pSpot->current = 0;
}

bool waitForNextFrame(uint32_t currentMillis)
{
    if (currentMillis - prevTimeMillis < (1000 / MAX_FPS))
        return true;
    return false;
}

void bike_died(Bike *pBike)
{
    pBike->diedTime = millis();
    pBike->lives = pBike->lives - 1;

    if (pBike->lives < 1)
    {
        pBike->speed = 0;
        return;
    }
    pBike->speed += 4;
    Serial.printf("Bike died %d)\n", pBike->lives);
    initBike(pBike, pBike->hue);
}

static void drawCircle(int x, int y, int radius, uint16_t color, uint16_t borderColor)
{
    matrix.fillCircle(x, y, radius, color);
    matrix.drawCircle(x, y, radius, borderColor);
}

void drawSpot(Spot *pSpot)
{
    //    uint8_t val = remap(pSpot->current, 3, 16, 255);
    // fade in and out with a sine wave
    uint8_t val = 255 * (sin((double)pSpot->current / (double)pSpot->phaseMillis * 2 * PI) + 1) / 2;

    color_t c = matrix.colorHSV(pSpot->hue, 255, val);
    drawCircle(pSpot->pos.x, pSpot->pos.y, pSpot->radius, 0, c);
}

void setup(void)
{
    Serial.begin(115200);
    // while (!Serial) delay(10);

    ProtomatterStatus status = matrix.begin();
    Serial.printf("Protomatter begin() status: %d\n", status);

    for (int8_t i = 0; i < N_BIKES; i++)
    {
        initBike(&bikes[i], color_hueForBikeIndex(i, N_BIKES));
        bikes[i].lives = 6;
        bikes[i].speed = 4; // random(6, 20);
    }

    for (int8_t i = 0; i < N_SPOTS; i++)
    {
        initSpot(&spots[i]);
    }

    prevTimeMillis = micros();
    Serial.printf("%d total bikes\n", N_BIKES);

    encoder_setup();
    chuck_setup();
}

bool isChuckActive(int i)
{
    return i < N_WIICHUCKS && chucks[i].active;
}

bool isEncoderActive(int i)
{
    return i < N_ENCODERS && encoders[i].active;
}

void loop()
{
    chuck_loop();
    encoder_loop();
    for (int i = 0; i < N_BIKES; i++)
    {
        Bike *bike = &bikes[i];

        if (isChuckActive(i) && bike->lives > 0)
        {
            bike_setJoyDirection(i, bike, chucks[i].x, chucks[i].y);
        }

        if (isEncoderActive(i))
        {
            auto newEncoderPosition = encoders[i].position;
            auto rotation = newEncoderPosition - bike->lastEncoderPosition;
            bike_rotateDirection(bike, rotation);
            bike->lastEncoderPosition = newEncoderPosition;
        }
    }

    uint32_t currentMillis = millis();
    if (waitForNextFrame(currentMillis))
        return;
    uint32_t dtMillis = currentMillis - prevTimeMillis;
    // Serial.printf("time %d\n", t);

    matrix.fillScreen(0x0);
    for (int i = 0; i < N_BIKES; i++)
    {
        Bike *bike = &bikes[i];

        auto respawnTime = currentMillis - bike->diedTime;
        auto isRespawning = respawnTime < secToMillis(RESPAWN_TIME);

        if (bike->lives < 1 || isRespawning)
        {
            bike->nextMoveTime = currentMillis;
            bike_setJoyDirection(i, bike, chucks[i].x, chucks[i].y);
            // Serial.printf("respawning %d %d\n", i, respawnTime);
            continue;
        }

        if (currentMillis < bike->nextMoveTime)
        {
            continue;
        }
        bike->nextMoveTime += secToMillis(1) / bike->speed;

        if (!isChuckActive(i) && !isEncoderActive(i))
        {
            if (ai_shouldChangeDirection(bike))
                ai_setRandomDirection(bike);
            continue;
        }
        moveBike(bike);
    }

    for (int i = 0; i < N_BIKES; i++)
    {
        Bike *bike = &bikes[i];
        if (isCollision(bike, 0))
        {
            bike_died(bike);
            chucks[i].active = 0;
        }
    }

    for (int i = 0; i < N_SPOTS; i++)
    {
        Spot *spot = &spots[i];
        spot->current += dtMillis;
        if (spot->current > spot->phaseMillis)
        {
            spot->current = 0;
            initSpot(spot);
        }
        drawSpot(spot);
    }

    drawBikes();
    matrix.show();
    prevTimeMillis = currentMillis;
}
