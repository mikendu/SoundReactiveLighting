#ifndef MILLISECOND_TIMER_H
#define MILLISECOND_TIMER_H

#include "CommonHeaders.h"

class MillisecondTimer
{

public:

    static void update();
    static float elapsed();
    static float elapsedSeconds();
    static unsigned long timestamp();

private:

    static unsigned long time;
    static unsigned long previous;
    
    static float elapsedTime;
    static bool validTime;

};


#endif