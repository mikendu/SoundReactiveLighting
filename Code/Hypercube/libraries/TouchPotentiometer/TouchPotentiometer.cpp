#include "TouchPotentiometer.h"


TouchPotentiometer::TouchPotentiometer(uint8_t address = DEFAULT_I2C_ADDRESS) : i2cAddress(address) 
{
}


void TouchPotentiometer::initialize()
{
    Wire.begin();
}

uint8_t TouchPotentiometer::get(bool readFromRegister = false)
{
    if (readFromRegister) 
    {
        Wire.beginTransmission(i2cAddress + 1);
        Wire.write('R');
        Wire.write(POT_ADDRESS);
        Wire.endTransmission();

        Wire.requestFrom(i2cAddress + 1, 1);
        if (Wire.available()) 
        {
            return Wire.read();
        } else 
        {
            return 0;
        }
    } 
    else 
    {
        Wire.requestFrom(i2cAddress, 1);
        if (Wire.available()) 
        {
            return Wire.read();
        } 
        else 
        {
            return 0;
        }
    }

}
void TouchPotentiometer::set(uint8_t value)
{
    Wire.beginTransmission(i2cAddress + 1);
    Wire.write('W');
    Wire.write(POT_ADDRESS);
    Wire.write(value);
    Wire.endTransmission();
}