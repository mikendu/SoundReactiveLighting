#ifndef POTENTIOMETER_H
#define POTENTIOMETER_H

class Potentiometer
{
public:
    Potentiometer(byte pin, byte activityThreshold)
    {
        this->pin = pin;
        this->threshold = activityThreshold;
    }

    void update()
    {
        uint16_t newValue = analogRead(this->pin);
        values[4] = values[3];
        values[3] = values[2];
        values[2] = values[1];
        values[1] = values[0];
        values[0] = newValue;
    }

    bool available() 
    {
        uint16_t newAverage = (values[0] + values[1]) / 2;
        uint16_t oldAverage = (values[2] + values[3] + values[4]) / 3;
        return abs(newAverage - oldAverage) >= threshold;
    }

    uint16_t rawAverage() 
    {
        uint32_t average = values[0] + values[1] + values[2] + values[3] + values[4];
        return average / 5;
    }

    float value() 
    {
        float average = this->rawAverage();
        return ((float)average) / 1023.0f;
    }

    float scaledValue()
    {
        float value = this->value();
        return value * value;
    }


private:
    uint8_t pin = 0;
    uint8_t threshold = 5;
    uint16_t values[5] = { 0, 0, 0, 0, 0 };

};


#endif