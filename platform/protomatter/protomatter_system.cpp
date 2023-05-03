#include <Arduino.h>
#include <system.h>
#include <renderer.h>
#include <game.h>
#include <input_wiichuck.h>
#include <input_encoder.h>

#define TICK_RATE_MILLIS 20
#define N_CONTROLLERS 4

Controller controllers[N_CONTROLLERS] = {};
sys_time_t Time;

static Game game = {};

const Controller *system_get_controller(uint8_t index) {
    if (index < N_CONTROLLERS) {
        return &controllers[index];
    }
    return NULL;
}

uint32_t system_get_millis(void) {
    return millis();
}

void system_delay(uint32_t ms) {
    delay(ms);
}

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    // while (!Serial) delay(100);
    Serial.printf("\n\nProtomatter System Setup\n");

    encoder_initAll();
    chuck_initAll();
    renderer_init();
    game = game_create();
    game.begin();
}

void loop() {
    auto time = system_get_millis();
    Time.delta = time - Time.time;
    if (Time.delta < TICK_RATE_MILLIS) return;
    Time.time = time;

    renderer_clear();
    encoder_updateAll();
    chuck_updateAll();

    for (size_t i = 0; i < N_CONTROLLERS; i++) {
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

    game.loop();
    renderer_present();
}