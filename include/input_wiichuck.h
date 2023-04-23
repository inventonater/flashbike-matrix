#include <WiiChuck.h>

#define MULTIPLEX_ADDR 0x70
#define N_WIICHUCKS 4

struct chuck_t
{
	Accessory data;
	int8_t x, y;
	bool active;
};
chuck_t chucks[N_WIICHUCKS] = {};

void chuck_setup()
{
	for (int i = 0; i < N_WIICHUCKS; i++)
	{
		auto *p = &chucks[i];
		p->data.addMultiplexer(MULTIPLEX_ADDR, i);
		p->data.begin();
		if (p->data.type == Unknown)
			p->data.type = NUNCHUCK;
	}
}

bool chuck_isActive(chuck_t *p)
{
	return p->data.getButtonZ() || p->data.getButtonC();
}

int mapJoystickValue(int value)
{
	int neutral = 128;
	int deadzone = 30; // Set deadzone size to 30 units

	if (value > neutral + deadzone)
	{
		return 1;
	}
	else if (value < neutral - deadzone)
	{
		return -1;
	}
	else
	{
		return 0;
	}
}

void chuck_loop()
{
	for (int i = 0; i < N_WIICHUCKS; i++)
	{
		chuck_t *p = &chucks[i];
		p->data.readData();
		if (!p->active)
		{
			if (chuck_isActive(p))
			{
				p->active = true;
				Serial.printf("Chuck wake: %d\n", i);
			}
			else
				continue;
		}

		p->x = mapJoystickValue(p->data.getJoyX());
		p->y = mapJoystickValue(p->data.getJoyY());

		// zero out the smaller of x or y
        if (abs(p->x) < abs(p->y))
            p->x = 0;
        else
            p->y = 0;
	}
}