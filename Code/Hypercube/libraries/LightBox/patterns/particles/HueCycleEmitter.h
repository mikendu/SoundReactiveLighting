#ifndef HUE_CYCLE_EMITTER_H
#define HUE_CYCLE_EMITTER_H

#include "ParticleEmitter.h"
#include "HSVOscillator.h"

class HueCycleEmitter : public ParticleEmitter
{

public:

    HueCycleEmitter(float particlesPerSecond, uint8_t offset) : ParticleEmitter(particlesPerSecond), hueOffset(offset)
    {
        colorOscillator = new HsvOscillator( { 67.2f }, { 5.75f, 0.3625f, 0.6875f }, { 7.35f, 0.275f, 0.475f });
        colorOscillator->setFlutter({ 9.0f, 0.4f, 0.6f});
    }

    virtual void updateSoundParameters(SpectrumAnalyzer& spectrum, DerivedParameters& parameters)
    {
        colorOscillator->setSpeed(parameters.getParameter(DerivedParameterType::OSCILLATOR_SPEED));
    }

    virtual void update(ParticleSystem* particleSystem, float deltaTime)
    {
        colorOscillator->update(deltaTime);
        ParticleEmitter::update(particleSystem, deltaTime);
    }

protected:
    virtual bool spawnParticle(ParticleSystem* particleSystem) 
    {
        CHSV color = colorOscillator->color();
        return particleSystem->spawn({ 3.0f, color.hue, color.sat, color.val, 10.0f, 15.0f, 1.0f, 0.5f }, 0.0f);  
    }

private:

    HsvOscillator* colorOscillator;
    uint8_t hueOffset;

};

#endif