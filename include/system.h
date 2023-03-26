#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdlib.h>
#include <time.h>

typedef struct System {
    uint32_t (*get_millis)(void);
    void (*delay)(uint32_t ms);
    void (*handle_input_events)(bool *quit, int *dx, int *dy);
} System;

extern System system_create();

#endif // SYSTEM_H
