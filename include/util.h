#include <Arduino.h>

int sign(const int8_t &x) { return x > 0 ? 1 : -1; }

uint32_t secToMicros(uint8_t s) {
    return s * 1000000L;
}

uint32_t secToMillis(uint8_t s) {
    return s * 1000L;
}
