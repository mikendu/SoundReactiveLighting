#ifndef LIGHT_BOX_BRIGHTNESS_CONTROLLER_H
#define LIGHT_BOX_BRIGHTNESS_CONTROLLER_H

#include <TouchPotentiometer.h>
#include "TouchButton.h"

class BrightnessController 
{

public:
    BrightnessController() : brightness(0), button(nullptr), potentiometer(nullptr)
    {
    }


    void initialize(TouchPotentiometer* potentiometer, TouchButton* button) 
    {
        this->potentiometer = potentiometer;
        this->button = button;
        
        this->button->initialize();
        this->potentiometer->initialize();
        this->brightness = potentiometer->get();
    }

    uint8_t update() 
    {
        this->brightness = potentiometer->get();
        if (!this->button->enabled()){
            this->brightness = 0;
        }

        return this->brightness;
    }

    uint8_t getBrightness() 
    {
        return this->brightness;
    }

private:

    TouchPotentiometer*     potentiometer;
    TouchButton*            button;
    uint8_t                 brightness;


};


#endif