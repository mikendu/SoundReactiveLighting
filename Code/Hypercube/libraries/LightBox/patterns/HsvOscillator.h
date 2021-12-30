#ifndef HSV_OSCILLATOR_H
#define HSV_OSCILLATOR_H

#include "Oscillator.h"

enum ChannelType
{
    HUE,
    SAT,
    VAL
};

class HsvOscillator
{

public:

    HsvOscillator(  OscillatorDefinition hueDef,
                    OscillatorDefinition satDef,
                    OscillatorDefinition valDef)
    {
        hueOscillator = new Oscillator(hueDef);
        satOscillator = new Oscillator(satDef);
        valOscillator = new Oscillator(valDef);
    }

    void setFlutter(OscillatorDefinition def) 
    {
        hueFlutter = new Oscillator(def);
    }

    void update(float deltaTime)
    { 
        hueOscillator->update(deltaTime);
        satOscillator->update(deltaTime);
        valOscillator->update(deltaTime);
        if (hueFlutter != nullptr)
        {
            hueFlutter->update(deltaTime);
        }
    }

    uint8_t hue()
    {
        uint8_t value = hueOscillator->value();
        if (hueFlutter != nullptr)
        {
            uint8_t flutterValue = hueFlutter->value();
            uint16_t wrapped = (value + flutterValue) % 256;
            return (uint8_t) wrapped;
        }
        else
        {
            return value;
        }
    }
    
    uint8_t sat()
    {
        return satOscillator->value();
    }
    
    uint8_t val()
    {
        return valOscillator->value();
    }

    CHSV color()
    {
        return CHSV(hue(), sat(), val());
    }

    void setSpeed(float speed)
    {
        hueOscillator->setSpeed(speed);
        satOscillator->setSpeed(speed);
        valOscillator->setSpeed(speed);
    }

    void setSpeed(ChannelType channel, float speed)
    {
        switch(channel)
        {
            case HUE:
                hueOscillator->setSpeed(speed);
                return;

            case SAT:
                satOscillator->setSpeed(speed);
                return;

            case VAL:
                valOscillator->setSpeed(speed);
                return;

            default:
                return;

        }
    }

private:
    Oscillator* hueOscillator;
    Oscillator* satOscillator;
    Oscillator* valOscillator;

    Oscillator* hueFlutter;
};


#endif