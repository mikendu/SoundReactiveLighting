#ifndef MILLISECOND_TIMER_H
#define MILLISECOND_TIMER_H

#include "CommonHeaders.h"

class MillisecondTimer
{

public:

    MillisecondTimer();
    void update();
    float elapsed();
    float elapsedSeconds();
    unsigned long timestamp();

private:

    unsigned long time;
    unsigned long previous;
    
    float millisecondsElapsed;
    float secondsElapsed;

};


#endif