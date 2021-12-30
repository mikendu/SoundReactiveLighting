#ifndef TOUCH_BUTTON_H
#define TOUCH_BUTTON_H

#define DOUBLE_TAP_THERSHOLD_MS 500

#include "Common.h"
#include "MillisecondTimer.h"

class TouchButton
{
public:
    TouchButton(uint8_t pin) : inputPin(pin), timer(false)
    {
    }

    void initialize()
    {
        pinMode(inputPin, INPUT);
    }

    void update() 
    {
        previouslyEnabled = isEnabled;
        isEnabled = (digitalRead(inputPin) == HIGH);
        tapCount = 0;

        if (isEnabled != previouslyEnabled) {
            tapCount = 1;
            
            float elapsed = timer.elapsed();
            if (elapsed < DOUBLE_TAP_THERSHOLD_MS) {
                tapCount = 2;
            }
            timer.start();
        }
    }

    bool enabled() 
    {
        return isEnabled;
    }

    bool tapped() 
    {
        return tapCount > 0;
    }

    bool doubleTapped()  
    {
        return tapCount > 1;
    }

private:
    uint8_t                 inputPin;
    bool                    isEnabled;
    bool                    previouslyEnabled;
    int                     tapCount;
    MillisecondTimer        timer;

};


#endif