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
                    HsvOscillator* colorOscillator,
                    float minLocation,
                    float maxLocation) :    ParticleEmitter(particlesPerSecond, minLocation, maxLocation, timeOffset), 
                                            hueOffset(offset), 
                                            oscillator(colorOscillator)
    {
    }

    virtual void updateSoundParameters(SpectrumAnalyzer& spectrum, DerivedParameters& parameters)
    {
        float multiplier = 0.125f * spectrum.getIntensity();
        float decreaseMultiplier = 1.0f - multiplier;
        float increaseMultiplier = 1.0f + multiplier;

        emissionInterval = baseInterval * decreaseMultiplier;
        minLife = 1.0f * increaseMultiplier;
        maxLife = 4.0f * increaseMultiplier;
        minSize = 5.0f * increaseMultiplier;
        maxSize = 12.0f * increaseMultiplier;
        minVel = 12.0f * increaseMultiplier;
        minVel = 18.0f * increaseMultiplier;
        
        // minLife = 2.0f;
        // maxLife = 4.0f;
        // minSize = 8.0f;
        // maxSize = 12.0f;
        // velocity = 15.0f;
    }

protected:
    virtual bool spawnParticle(ParticleSystem* particleSystem) 
    {
        CHSV color = oscillator->color();
        short hue = color.h;
        hue = (hue + hueOffset + 256) % 256;
        color.h = hue;

        // float location = random(0.0f, 50.0f);
        // float size = random(8.0f, 12.0f);
        // float life = random(2.0f, 4.0f);
        
        float location = random(locationMin, locationMax);
        float size = random(minSize, maxSize);
        float life = random(minLife, maxLife);
        float speed = random(minVel, maxVel);
        float direction = (randomFloat() >= 0.5f) ? 1.0f : -1.0f;
        float velocity = direction * speed;

        // float location = 0;
        // float size = 10.0f;
        // float life = 3.0f;
        // float velocity = 15.0f;        

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
                DEFAULT_BRIGHTNESS_PROFILE,
                locationMin,
                locationMax
            }, 
            location
        );  
    }

private:
    HsvOscillator* oscillator;
    signed char hueOffset;

    float minLife;
    float maxLife;
    float minSize;
    float maxSize;
    float minVel;
    float maxVel;

};

#endif