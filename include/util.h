#include <Arduino.h>

uint32_t secToMicros(uint8_t s) {
    return s * 1000000L;
}

uint32_t secToMillis(uint8_t s) {
    return s * 1000L;
}
