#include <system.h>
#include <renderer.h>
#include <game.h>
#include <Arduino.h>

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

void setup()
{
    const System system = system_create();
    const Renderer renderer = renderer_create();
    const Game game = game_create();
    game.begin(system, renderer);
}

void loop()
{
}