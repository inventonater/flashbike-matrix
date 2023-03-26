#include <system.h>
#include <Arduino.h>

uint32_t system_get_millis(void) {
    return millis();
}

void system_delay(uint32_t ms) {
    delay(ms);
}

System system_create() {
    System system;
    system.get_millis = system_get_millis;
    system.delay = system_delay;
    return system;
}

