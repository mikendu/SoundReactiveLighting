#include "MillisecondTimer.h"

MillisecondTimer::MillisecondTimer(bool continuous = false):   time(0), 
                                                                previous(0), 
                                                                millisecondsElapsed(0.0f),
                                                                continuousMode(continuous) {

}

float MillisecondTimer::update()
{
    previous = time;
    time = micros();
    
    float elapsedMicro = time - previous;
    millisecondsElapsed = elapsedMicro / 1000.0f;
    return millisecondsElapsed;
}

float MillisecondTimer::elapsed()
{
    if (!continuousMode)
        return update();

    return millisecondsElapsed;
}

void MillisecondTimer::start() 
{
    time = micros();
}