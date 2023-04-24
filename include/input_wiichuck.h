#include <WiiChuck.h>

#define MULTIPLEX_ADDR 0x70
#define N_WIICHUCKS 4

struct chuck_t
{
	Accessory data;
	int x, y;
	bool active;
};
chuck_t chucks[N_WIICHUCKS] = {};

bool chuck_isActive(int i)
{
    return i < N_WIICHUCKS && chucks[i].active;
}

void chuck_setup()
{
    for (int i = 0; i < N_WIICHUCKS; i++)
	{
		auto *p = &chucks[i];

        p->data = Accessory();
        p->x = 0;
        p->y = 0;
        p->active = false;

		p->data.addMultiplexer(MULTIPLEX_ADDR, i);
		p->data.begin();
		if (p->data.type == Unknown) p->data.type = NUNCHUCK;
	}
}

bool chuck_anyButtonPressed(chuck_t *p) { return p->data.getButtonZ() || p->data.getButtonC(); }

int mapJoystickValue(int value)
{
	int neutral = 128;
	int deadzone = 100;
    if (value > neutral + deadzone) return 1;
    else if (value < neutral - deadzone) return -1;
    return 0;
}

void chuck_loop()
{
	for (int i = 0; i < N_WIICHUCKS; i++)
	{
		chuck_t *p = &chucks[i];
		p->data.readData();

		if (!p->active)
		{
            if (!chuck_anyButtonPressed(p)) continue;
            else {
                p->active = true;
                Serial.printf("Chuck wake: %d\n", i);
            }
        }

		p->x = mapJoystickValue(p->data.getJoyX());
		p->y = mapJoystickValue(p->data.getJoyY());

        if (abs(p->x) < abs(p->y)) p->x = 0;
        else p->y = 0;
	}
}