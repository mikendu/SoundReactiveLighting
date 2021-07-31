#ifndef LIGHT_BOX_TIMERS_H
#define LIGHT_BOX_TIMERS_H

#include "MillisecondTimer.h"
#include "LightUtils.h"

#define HOLD_THRESHOLD 0.05f

class DecayTimer
{

public:
    DecayTimer(MillisecondTimer& baseTimer, float decay, float pow = 1.0f):  
                                                timer(baseTimer),
                                                decayTimeMS(decay),
                                                power(pow),
                                                counter(0.0f),
                                                currentValue(0.0f),
                                                startValue(0.0f)
    {
        
    }


    void update(float input)
    {
        if(input > currentValue)
        {
            startValue = input;
            counter = 0.0f;
        }
        
        if(counter <= decayTimeMS) 
        {

            counter += timer.elapsed();
            float linearVal = counter / decayTimeMS;
            currentValue = LightUtils::lerp(startValue, 0.0f, linearVal);
            //currentValue = pow(currentValue, power);
        }
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
    MillisecondTimer& timer;
};

class HoldTimer
{

public:
    HoldTimer(MillisecondTimer& baseTimer, float value, float holdTime, float decayTime) :   
                                                                timer(baseTimer),
                                                                holdValue(value),
                                                                holdTimeMS(holdTime),
                                                                decayTimeMS(decayTime),
                                                                counter(0.0f),
                                                                currentValue(0.0f)
    {
        totalDuration = holdTime + decayTime;
    }

    void update(float input)
    {
        if (abs(input - holdValue) > HOLD_THRESHOLD)
        {
            counter = 0.0f;
            currentValue = 0.0f;
        }

        if(counter <= totalDuration)
        {
            counter += timer.elapsed();
            
            if(counter >= holdTimeMS)
            {
                float linearVal = (counter - holdTimeMS) / decayTimeMS;
                currentValue = LightUtils::lerp(0, 1, linearVal);
            }
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
    float totalDuration;
    MillisecondTimer& timer;


};

class SinusoidTimer
{

public:

    SinusoidTimer(MillisecondTimer& baseTimer, float period): timer(baseTimer), period(period)
    {

    }

    float update(float multiplier = 1.0f)
    {
        float twoPI = 2 * M_PI;
        counter += ((twoPI * timer.elapsed() / period) * multiplier);
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
    MillisecondTimer& timer;

};


#endif