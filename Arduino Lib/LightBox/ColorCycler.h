#ifndef COLOR_CYCLER_H
#define COLOR_CYCLER_H

#include "CommonHeaders.h"
#include "MillisecondTimer.h"

class ColorCycler 
{

public:

    ColorCycler(MillisecondTimer& baseTimer): timer(baseTimer) {}

    CHSV randomColor() 
    {
        
        float h, s, v;
        cycleCounter += (this->timer.elapsedSeconds() * 10.0f);
        h = (0.5f * sin(cycleCounter)) + 0.5f;
        s = (0.5f * cos(cycleCounter * 6.3f)) + 0.5f;
        v = (0.5f * sin(cycleCounter * 7.6f)) + 0.5f;

        if(cycleCounter > 100.0f)
            cycleCounter -= 100.0f;

        uint8_t hue, sat, val;
        hue = (uint8_t)(h * 255);
        sat = (uint8_t)(s * 255);
        val = (uint8_t)(v * 255);

        return CHSV(hue, sat, val);
    }

private:

    float cycleCounter;
    MillisecondTimer& timer;

};




#endif