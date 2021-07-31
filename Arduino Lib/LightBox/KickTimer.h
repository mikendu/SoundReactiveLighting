#ifndef KICK_TIMER_H
#define KICK_TIMER_H

#include "LightUtils.h"
#include "MillisecondTimer.h"


class KickTimer
{

public:
    KickTimer(MillisecondTimer& baseTimer, float dip, float flash, float fade): milliTimer(baseTimer),
                                                                                timer(0),
                                                                                power(0),
                                                                                dipDuration(dip),
                                                                                flashDuration(flash),
                                                                                fadeDuration(fade),
                                                                                totalDuration(dip + flash + fade),
                                                                                colorSat(1.0f),
                                                                                colorVal(1.0f)

    {
    }


    void update(float input)
    {
        if(input > 0.0f)
        {
            timer = 0.0f;
            colorSat = 1.0f;
            colorVal = 1.0f;
            power = input;
        }


        if(timer < totalDuration)
        {

            if(timer < dipDuration)
            {
                colorSat = 1.0f;
                colorVal = 1.0f - power;
            }
            else if(timer < dipDuration + flashDuration)
            {
                colorSat = 1.0f - power;
                colorVal = 1.0f;
            }            
            else if(timer < dipDuration + flashDuration + fadeDuration)
            {
                float progress = (timer - (dipDuration + flashDuration)) / fadeDuration;

                colorSat = LightUtils::lerp(1.0f - power, 1.0f, progress);
                colorVal = LightUtils::lerp(1.0f - power, 1.0f, pow(progress, 1.0f));
            }
            else
            {        
                colorSat = 1.0f;
                colorVal = 1.0f;
            }

            timer += this->milliTimer.elapsed();
            //timer += 0.0001f;
        }
    }

    bool enabled() { return timer < totalDuration; }
    float saturation() { return colorSat; }
    float value() { return colorVal; }

#ifdef DEBUG_MODE
    float testValue() 
    {
        return timer;    
    }
#endif

private:
    float power;
    float timer;
    float dipDuration;
    float flashDuration;
    float fadeDuration;
    float totalDuration;
    float colorSat;
    float colorVal;
    MillisecondTimer& milliTimer;
};



#endif