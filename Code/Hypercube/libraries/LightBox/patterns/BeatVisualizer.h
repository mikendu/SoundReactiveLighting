#ifndef BEAT_VISUALIZER_H
#define BEAT_VISUALIZER_H

#include "DerivedParameters.h"
#include "SpectrumAnalyzer.h"
#include "NeopixelStrip.h"

#define NODE_INTERVAL 60.0f
#define MIN_RADIUS 5.0f
#define MAX_RADIUS 30.0f
#define MAX_VALUE 32

class BeatVisualizer
{

public:

    void updateSoundParameters(SpectrumAnalyzer& spectrum)
    {        
        #ifdef SOUND_REACTIVE
            
            // Kick
            if (spectrum.getOnset(SoundBand::KICK)) 
            {
                float energy = spectrum.getEnergy(SoundBand::KICK);
                radius = lerp(MIN_RADIUS, MAX_RADIUS, energy);
                value = MAX_VALUE;
                
                //value = round(MAX_VALUE * energy);
                //radius = MAX_RADIUS;
            }
            else
            {
                value = 0;
                radius = MIN_RADIUS;
            }

        #else
            value = 0;
            radius = MIN_RADIUS;
        #endif
    }

    void show(NeopixelStrip& strip) 
    {
        for (uint16_t i = 0; i < strip.length(); i++) 
        {
            float nearestNode = round(i / NODE_INTERVAL) * NODE_INTERVAL;
            float distance = 1.0f - constrain(abs(nearestNode - i) / radius, 0.0f, 1.0f);
            uint8_t scaledValue = round(value * distance);
            scaledValue = ease8InOutCubic(scaledValue);
            strip[i] += CRGB(scaledValue, scaledValue, scaledValue);
        }
    }

private:

    uint8_t value;
    float radius;

};


#endif