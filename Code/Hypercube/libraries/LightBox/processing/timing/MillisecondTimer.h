#ifndef MILLISECOND_TIMER_H
#define MILLISECOND_TIMER_H

#include "Common.h"

class MillisecondTimer
{

public:

    MillisecondTimer(bool continuous = false);
    float update();
    float elapsed();
    void start();

private:

    unsigned long time;
    unsigned long previous;
    float millisecondsElapsed;
    bool continuousMode;
    

};


#endif