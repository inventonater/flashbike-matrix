#include <Arduino.h>

// declare the switch pin D6

int onSwitch = 6;

void halleffect_setup()
{
    // declare switch pin as input_pullup to use internal resistor
    // pinMode(onSwitch, INPUT_PULLUP);
    // digital hall sensor (A3144 402 type) acts as on/off switch
    pinMode(onSwitch, INPUT_PULLUP);
}

void halleffect_loop()
{
    // read the pushbutton value into a variable
    int sensorVal = digitalRead(onSwitch);

    // Keep in mind the pullup means the pushbutton's
    // logic is inverted. It goes HIGH when it's open,
    // and LOW when it's pressed. Turn on pin 13 when the
    // button's pressed, and off when it's not:

    if (sensorVal == HIGH)
    { 
        // the button has been pressed/switch has been closed
        // while the switch is closed, make the led go off and the panel transparent
        Serial.printf("hall: %d\n", sensorVal);
    }

    else if (sensorVal == LOW)
    {
        Serial.printf("hall: %d\n", sensorVal);
    }
}

