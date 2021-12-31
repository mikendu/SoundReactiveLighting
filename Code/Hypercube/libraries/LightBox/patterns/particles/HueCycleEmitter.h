#ifndef HUE_CYCLE_EMITTER_H
#define HUE_CYCLE_EMITTER_H

#include "ParticleEmitter.h"
#include "HSVOscillator.h"

#define DEFAULT_HARDNESS 1.0f
#define DEFAULT_BRIGHTNESS_PROFILE 0.75f

class HueCycleEmitter : public ParticleEmitter
{

public:

    HueCycleEmitter(float particlesPerSecond,
                    float timeOffset, 
                    signed char offset, 
                    HsvOscillator* colorOscillator) : ParticleEmitter(particlesPerSecond, timeOffset), hueOffset(offset), oscillator(colorOscillator)
    {
    }

    virtual void updateSoundParameters(SpectrumAnalyzer& spectrum, DerivedParameters& parameters)
    {
        // float halfIntensity = 0.5f * spectrum.getIntensity();
        // float decreaseMultiplier = 1.0f - halfIntensity;
        // float increaseMultiplier = 1.0f + halfIntensity;

        // emissionInterval = baseInterval * decreaseMultiplier;
        // minLife = 2.0f * decreaseMultiplier;
        // maxLife = 4.0f * decreaseMultiplier;
        // minSize = 8.0f * decreaseMultiplier;
        // maxSize = 8.0f * decreaseMultiplier;
        // velocity = 15.0f * increaseMultiplier;
        
        // minLife = 2.0f;
        // maxLife = 4.0f;
        // minSize = 8.0f;
        // maxSize = 12.0f;
        // velocity = 15.0f;
    }

protected:
    virtual bool spawnParticle(ParticleSystem* particleSystem, uint16_t stripLength) 
    {
        CHSV color = oscillator->color();
        short hue = color.h;
        hue = (hue + hueOffset + 256) % 256;
        color.h = hue;

        float location = random(0.0f, stripLength);
        float size = random(8.0f, 12.0f);
        float life = random(2.0f, 4.0f);
        float velocity = 15.0f;
        
        // float location = 0;
        // float size = 10.0f;
        // float life = 3.0f;

        return particleSystem->spawn
        (
            { 
                life, 
                color.hue, 
                color.sat, 
                color.val, 
                size, 
                velocity, 
                DEFAULT_HARDNESS, 
                DEFAULT_BRIGHTNESS_PROFILE 
            }, 
            location
        );  
    }

private:
    HsvOscillator* oscillator;
    signed char hueOffset;

    // float minLife;
    // float maxLife;
    // float minSize;
    // float maxSize;
    // float velocity;

};

#endif