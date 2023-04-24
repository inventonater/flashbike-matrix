#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdlib.h>
#include <time.h>

typedef struct Controller
{
    int x, y;
    int32_t encoder_position;
    bool active;
} Controller;

typedef struct System {
    uint32_t (*get_millis)(void);
    void (*delay)(uint32_t ms);
    const Controller* (*get_controller)(uint8_t index);
} System;

extern System system_create();

#endif // SYSTEM_H
