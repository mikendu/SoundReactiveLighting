#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include "DerivedParameters.h"
#include "SpectrumAnalyzer.h"

class ParticleSystem;


class ParticleEmitter
{

public:
    

    ParticleEmitter(float particlesPerSecond, 
                    float minLocation, 
                    float maxLocation) : ParticleEmitter(particlesPerSecond, minLocation, maxLocation, 0.0f)
    {
    }


    ParticleEmitter(float particlesPerSecond, 
                    float minLocation, 
                    float maxLocation, 
                    float timeOffset) : locationMin(minLocation), locationMax(maxLocation)
    {
        emissionInterval = 1.0f / particlesPerSecond;
        emissionCounter = emissionInterval + timeOffset;
        baseInterval = emissionInterval;
    }

    virtual void updateSoundParameters(SpectrumAnalyzer& spectrum, DerivedParameters& parameters)
    {
    }

    virtual void update(ParticleSystem* particleSystem, float deltaTime)
    {          
        if (emissionCounter <= 0.0f)
        {
            if (spawnParticle(particleSystem))
                emissionCounter = emissionInterval;
        }
        else
        {
            emissionCounter -= deltaTime;
        }
    }


protected:
    virtual bool spawnParticle(ParticleSystem* particleSystem) = 0;

    float baseInterval;
    float emissionInterval;
    float emissionCounter;
    float locationMin;
    float locationMax;

};

#endif