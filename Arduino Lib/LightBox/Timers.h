#ifndef LIGHT_BOX_TIMERS_H
#define LIGHT_BOX_TIMERS_H

#include "MillisecondTimer.h"
#include "LightUtils.h"

#define HOLD_THRESHOLD 0.01f

class DecayTimer
{

public:
    DecayTimer(float decay, float pow = 1.0f):  decayTimeMS(decay),
                                                power(pow),
                                                counter(0.0f),
                                                currentValue(0.0f),
                                                startValue(0.0f)
    {
        
    }


    void update(float input)
    {
        if(input >= currentValue)
        {
            startValue = input;
            counter = 0.0f;
        }

        counter += MillisecondTimer::elapsed();
        float linearVal = counter / decayTimeMS;
        currentValue = LightUtils::lerp(startValue, 0.0f, linearVal);
        //currentValue = pow(currentValue, power);
    }

    float value()
    {
        return this->currentValue;
    }

private:
    float currentValue;
    float counter;
    float decayTimeMS;
    float power;
    float startValue;
};

class HoldTimer
{

public:
    HoldTimer(float value, float holdTime, float decayTime) :   holdValue(value),
                                                                holdTimeMS(holdTime),
                                                                decayTimeMS(decayTime),
                                                                counter(0.0f),
                                                                currentValue(0.0f)
    {

    }

    void update(float input)
    {
        if (abs(input - holdValue) > HOLD_THRESHOLD)
        {
            counter = 0.0f;
            currentValue = 0.0f;
        }

        counter += MillisecondTimer::elapsed();
        
        if(counter >= holdTimeMS)
        {
            float linearVal = (counter - holdTimeMS) / decayTimeMS;
            currentValue = LightUtils::lerp(0, 1, linearVal);
        }
    }

    float value()
    {
        return this->currentValue;
    }

private:

    float currentValue;
    float holdValue;
    float holdTimeMS;
    float decayTimeMS;
    float counter;


};

class SinusoidTimer
{

public:

    SinusoidTimer(float period): period(period)
    {

    }

    float update(float multiplier = 1.0f)
    {
        float twoPI = 2 * M_PI;
        counter += ((twoPI * MillisecondTimer::elapsed() / period) * multiplier);
        if (counter > period)
            counter -= period;

        currentValue = 0.5f + (0.5f * sin(counter));
    }

    float value()
    {
        return this->currentValue;
    }

    float scaledValue(float minVal, float maxVal)
    {
        return LightUtils::lerp(minVal, maxVal, this->value());
    }

private:

    float period;
    float counter;
    float currentValue;

};


#endif