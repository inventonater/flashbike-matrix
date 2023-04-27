#include <Arduino.h>
#include <system.h>
#include <renderer.h>
#include <game.h>
#include <input_wiichuck.h>
#include <input_encoder.h>

#define N_CONTROLLERS 4

Controller controllers[N_CONTROLLERS] = {};
Controller null_controller = {};

const Controller *system_get_controller(uint8_t index) {
    if (index < N_CONTROLLERS) {
        return &controllers[index];
    }
    return &null_controller;
}

uint32_t system_get_millis(void) {
    return millis();
}

void system_delay(uint32_t ms) {
    delay(ms);
}

Game g = {};

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    // while (!Serial) delay(100);
    Serial.printf("\n\nProtomatter System Setup\n");

    encoder_initAll();
    chuck_initAll();

    renderer_init();

    g = game_create();
    g.begin();
}

void loop() {
    renderer_clear();

    encoder_updateAll();
    chuck_updateAll();

    for (uint8_t i = 0; i < N_CONTROLLERS; i++) {
        Controller *c = &controllers[i];
        c->active = false;

        if (encoder_isActive(i)) {
            c->active = true;
            c->position = encoders[i].position;
        }

        if (chuck_isActive(i)) {
            c->active = true;
            c->x = chucks[i].x;
            c->y = chucks[i].y;
        }
    }

    g.loop();
    renderer_present();
}