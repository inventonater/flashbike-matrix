#ifndef SYSTEM_H
#define SYSTEM_H

typedef struct Controller
{
    int8_t x, y;
    int32_t position;
    bool active;
} Controller;

typedef struct
{
    uint32_t time;
    uint32_t delta;
} sys_time_t;
extern sys_time_t Time;

uint32_t system_get_millis(void);
void system_delay(uint32_t ms);
const Controller *system_get_controller(uint8_t index);

#endif // SYSTEM_H
