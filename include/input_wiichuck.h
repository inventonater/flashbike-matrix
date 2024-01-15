#include <WiiChuck.h>
#include <Wire.h>
#include <system.h>

#define MULTIPLEX_ADDR 0x70
#define N_WIICHUCKS 4

struct chuck_t {
    bool active;
    Accessory data;
    int x, y;
};
chuck_t chucks[N_WIICHUCKS] = {};

bool multiplexerPresent = false;
bool isMultiplexerPresent() {
    Wire.beginTransmission(MULTIPLEX_ADDR);
    Wire.write(0x00); // Point to the register 0x00
    byte status = Wire.endTransmission();
    Serial.printf("isMultiplexerPresent status: %d\n", status);

    if (status != 0) {
        return false; // Error occurred during the transmission
    }

    Wire.requestFrom(MULTIPLEX_ADDR, 1); // Request 1 byte from the multiplexer
    if (Wire.available()) {
        byte registerValue = Wire.read();

        Serial.printf("isMultiplexerPresent registerValue: %d\n", registerValue);
        return registerValue == 0x00; // Check if the register value is 0x00
    }

    return false; // No data available from the multiplexer
}

bool chuck_isActive(int i) { return i < N_WIICHUCKS && chucks[i].active; }

bool chuck_anyButtonPressed(chuck_t *p) { return p->data.getButtonZ() || p->data.getButtonC(); }

int mapJoystickValue(int value) {
    int neutral = 128;
    int deadzone = 100;
    if (value > neutral + deadzone) return 1;
    else if (value < neutral - deadzone) return -1;
    return 0;
}

void chuck_init(uint8_t i) {
    Wire.begin();
    multiplexerPresent = isMultiplexerPresent();

    auto *p = &chucks[i];
    p->data = Accessory();
    p->x = 0;
    p->y = 0;
    p->active = false;

    if(multiplexerPresent){
        p->data.addMultiplexer(MULTIPLEX_ADDR, i);
        p->data.begin();
        if (p->data.type == Unknown) p->data.type = NUNCHUCK;
    }
}

void chuck_update(uint8_t i) {
    if(!multiplexerPresent) return;

    chuck_t *p = &chucks[i];
    p->data.readData();

    if (!p->active) {
        if (!chuck_anyButtonPressed(p)) return;
        p->active = true;
        Serial.printf("Chuck wake: %d\n", i);
    }

    p->x = mapJoystickValue(p->data.getJoyX());
    p->y = mapJoystickValue(p->data.getJoyY());

    if (abs(p->x) < abs(p->y)) p->x = 0;
    else p->y = 0;
}

void chuck_initAll() { for (int i = 0; i < N_WIICHUCKS; i++) chuck_init(i); }

void chuck_updateAll() { for (int i = 0; i < N_WIICHUCKS; i++) chuck_update(i); }