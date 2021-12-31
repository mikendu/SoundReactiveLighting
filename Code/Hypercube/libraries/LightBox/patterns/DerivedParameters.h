#ifndef DERIVED_PARAMETERS_H
#define DERIVED_PARAMETERS_H

#include "SpectrumAnalyzer.h"

enum DerivedParameterType
{
    PARTICLE_SPEED,
    PARTICLE_INTENSITY,
    OSCILLATOR_SPEED,
};


class DerivedParameters
{
public:

    void compute(SpectrumAnalyzer& spectrum)
    {
        #ifdef SOUND_REACTIVE
            particleSpeed = 1.0f + (16.0f * pow(spectrum.getEnergy(SoundBand::KICK), 2.0f));
            particleIntensity = 1.0f + spectrum.getIntensity();
            oscillatorSpeed = 1.0f + (2.0f * spectrum.getIntensity());
        #else
            particleSpeed = 1.0f;
            particleIntensity = 1.0f;
            oscillatorSpeed = 1.0f;
        #endif
    }

    float getParameter(DerivedParameterType type)
    {
        switch (type)
        {
            case PARTICLE_SPEED:
                return particleSpeed;

            case PARTICLE_INTENSITY:
                return particleIntensity;

            case OSCILLATOR_SPEED:
                return oscillatorSpeed;

            default:
                return 0;
        }
    }

private:
    float   particleSpeed;
    float   particleIntensity;
    float   oscillatorSpeed;
};


#endif