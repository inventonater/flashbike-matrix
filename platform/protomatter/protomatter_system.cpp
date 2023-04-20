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

void system_handle_input_events(bool *quit, int *dx, int *dy) {
    
}

System system_create()
{
    System system;
    system.get_millis = system_get_millis;
    system.delay = system_delay;
    system.handle_input_events = system_handle_input_events;
    return system;
}

void setup()
{
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    while (!Serial) delay(100);
    Serial.printf("\n\nProtomatter System Setup\n");
    
    const System system = system_create();
    const Renderer renderer = renderer_create();
    const Game game = game_create();
    game.begin(system, renderer);
}

void loop()
{
    Serial.println("loop!");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);                // wait for a half second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(500);                // wait for a half second
}