#ifndef PARTICLE_EMITTER_H
#define PARTICLE_EMITTER_H

#include "DerivedParameters.h"
#include "SpectrumAnalyzer.h"

class ParticleSystem;


class ParticleEmitter
{

public:

    ParticleEmitter(float particlesPerSecond)
    {
        emissionInterval = 1.0f / particlesPerSecond;
        emissionCounter = emissionInterval;
    }

    virtual void updateSoundParameters(SpectrumAnalyzer& spectrum, DerivedParameters& parameters) = 0;

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

    float emissionInterval;
    float emissionCounter;

};

#endif