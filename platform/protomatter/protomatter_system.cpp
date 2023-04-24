#include <Arduino.h>
#include <system.h>
#include <renderer.h>
#include <game.h>
#include <input_wiichuck.h>
#include <input_encoder.h>

uint32_t system_get_millis(void)
{
    return millis();
}

void system_delay(uint32_t ms)
{
    delay(ms);
}

System system_create()
{
    System system;
    system.get_millis = system_get_millis;
    system.delay = system_delay;
    return system;
}

Controller controllers[N_CONTROLLERS] = {};

System s = {};
Renderer r = {};
Game g = {};

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // turn the LED on (HIGH is the voltage level)
    // while (!Serial) delay(100);
    Serial.printf("\n\nProtomatter System Setup\n");

    encoder_setup();
    chuck_setup();

    s = system_create();
    r = renderer_create();
    r.init();

    g = game_create();
    g.begin(s, r);
}

void loop()
{
    Serial.println("loop!");
    r.clear();
    encoder_loop();
    chuck_loop();
    for (uint8_t i = 0; i < N_CONTROLLERS; i++)
    {
        Controller *c = &controllers[i];
        c->active = false;

        if (encoder_isActive(i))
        {
            c->active = true;
            c->encoder_position = encoders[i].encoder_position;
        }

        if (chuck_isActive(i))
        {
            c->active = true;
            c->x = chucks[i].x;
            c->y = chucks[i].y;
        }
    }

    g.loop(s, r);
    r.present();
}