#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdlib.h>
#include <time.h>

#define N_CONTROLLERS 4

typedef struct Controller
{
    int x, y;
    int32_t encoder_position;
    bool active;
} Controller;

extern Controller controllers[N_CONTROLLERS];

typedef struct System {
    uint32_t (*get_millis)(void);
    void (*delay)(uint32_t ms);
} System;

extern System system_create();

#endif // SYSTEM_H
