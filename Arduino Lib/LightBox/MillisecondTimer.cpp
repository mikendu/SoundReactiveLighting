#include "MillisecondTimer.h"

unsigned long MillisecondTimer::time = 0;
unsigned long MillisecondTimer::previous = 0;

float MillisecondTimer::elapsedTime = 0.0f;
bool MillisecondTimer::validTime = false;

void MillisecondTimer::update()
{
    validTime = false;
    previous = time;
    time = micros();

    // Overflow
    if(time < previous) 
    {
        previous = time;
        return;
    }
}

float MillisecondTimer::elapsed()
{
    if(validTime)
        return elapsedTime;

    float elapsedMicro = time - previous;
    elapsedTime = elapsedMicro / 1000.0f;
    return elapsedTime;
}

float MillisecondTimer::elapsedSeconds()
{
    return elapsed() / 1000.0f;
}

unsigned long MillisecondTimer::timestamp()
{
    return time;
}