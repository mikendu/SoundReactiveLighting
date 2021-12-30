#ifndef OSCILLATOR_H
#define OSCILLATOR_H

#include "Common.h"



struct OscillatorDefinition
{
    float period;
    float min = 0.0f;
    float max = 1.0f;
};

// ---- class ---- //

class Oscillator
{

public:

    Oscillator(OscillatorDefinition def) : Oscillator(def.period, def.min, def.max)
    {
    }

    Oscillator(float periodSec, float min = 0.0f, float max = 1.0f) : period(periodSec), speed(1.0f)
    {        
        this->min = round(255 * min);
        this->max = round(255 * max);
        range = this->max - this->min;
    }

    void update(float deltaTime)
    {
        counter += (deltaTime * speed);        
        if (counter >= period) 
            counter -= period;

        uint8_t time = 255 * (counter / period);
        uint8_t rawValue = quadwave8(time);
        val = qadd8(scale8(rawValue, range), min);
    }

    uint8_t value()
    {
        return val;
    }

    void setSpeed(float speed)
    {
        this->speed = speed;
    }

private:
    uint8_t val;
    uint8_t min;
    uint8_t max;
    uint8_t range;

    float counter;
    float period;
    float speed;
};


#endif