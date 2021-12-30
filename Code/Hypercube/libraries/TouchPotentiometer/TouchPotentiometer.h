#ifndef TOUCH_POT_H
#define TOUCH_POT_H

#include "Arduino.h"
#include "Wire.h"

#define DEFAULT_I2C_ADDRESS 8
#define POT_ADDRESS 1

class TouchPotentiometer
{

public:
    TouchPotentiometer(uint8_t address = DEFAULT_I2C_ADDRESS);

    void        initialize();
    uint8_t     get(bool readFromRegister = false);
    void        set(uint8_t value);

private: 
    uint8_t     i2cAddress;

};




#endif
