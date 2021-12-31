#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include "DerivedParameters.h"
#include "SpectrumAnalyzer.h"

class ParticleSystem;


class ParticleEmitter
{

public:
    

    ParticleEmitter(float particlesPerSecond) : ParticleEmitter(particlesPerSecond, 0.0f)
    {
    }


    ParticleEmitter(float particlesPerSecond, float timeOffset)
    {
        emissionInterval = 1.0f / particlesPerSecond;
        emissionCounter = emissionInterval + timeOffset;
        baseInterval = emissionInterval;
    }

    virtual void updateSoundParameters(SpectrumAnalyzer& spectrum, DerivedParameters& parameters)
    {
    }

    virtual void update(ParticleSystem* particleSystem, float deltaTime, uint16_t stripLength)
    {          
        if (emissionCounter <= 0.0f)
        {
            if (spawnParticle(particleSystem, stripLength))
                emissionCounter = emissionInterval;
        }
        else
        {
            emissionCounter -= deltaTime;
        }
    }


protected:
    virtual bool spawnParticle(ParticleSystem* particleSystem, uint16_t stripLength) = 0;

    float baseInterval;
    float emissionInterval;
    float emissionCounter;

};

#endif