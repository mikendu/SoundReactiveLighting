#include "MillisecondTimer.h"

MillisecondTimer::MillisecondTimer():   time(0), 
                                        previous(0), 
                                        millisecondsElapsed(0.0f), 
                                        secondsElapsed(0.0f) {

}

void MillisecondTimer::update()
{
    previous = time;
    time = micros();

    // Overflow
    if(time < previous) 
    {
        previous = time;
        return;
    }
    
    float elapsedMicro = time - previous;
    millisecondsElapsed = elapsedMicro / 1000.0f;
    secondsElapsed = millisecondsElapsed / 1000.0f;
}

float MillisecondTimer::elapsed()
{
    return millisecondsElapsed;
}

float MillisecondTimer::elapsedSeconds()
{
    return secondsElapsed;
}

unsigned long MillisecondTimer::timestamp()
{
    return time;
}