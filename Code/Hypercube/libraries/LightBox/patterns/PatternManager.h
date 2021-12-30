#ifndef PATTERN_MANAGER_H
#define PATTERN_MANAGER_H

#include "ParticleSystem.h"
#include "HueCycleEmitter.h"
#include "ColorParameters.h"
#include "MillisecondTimer.h"
#include "Common.h"

class PatternManager
{
public:

    PatternManager(SpectrumAnalyzer& analyzer, NeopixelStrip& ledStrip) : spectrum(analyzer), strip(ledStrip)
    {
        timer = new MillisecondTimer(true);
        timer->start();

        analyzer.addCallback(std::bind(&PatternManager::updateSoundParameters, this));
        primaryEmitter = new HueCycleEmitter(1.0f, 0);
        particleSystem.addEmitter(primaryEmitter);
    }

    void updateSoundParameters()
    {
        derivedParameters.compute(spectrum);
        colorParameters.updateSoundParameters(spectrum);
        particleSystem.updateSoundParameters(spectrum, derivedParameters);
    }

    void update()
    {
        float deltaTime = timer->update() / 1000.0f;
        colorParameters.update(deltaTime);
        particleSystem.update(deltaTime, strip.length());
        particleSystem.show(strip, colorParameters);
    }


private:

    MillisecondTimer*   timer;
    SpectrumAnalyzer&   spectrum;
    NeopixelStrip&      strip;
    DerivedParameters   derivedParameters;
    ColorParameters     colorParameters;
    
    // Particles
    ParticleSystem      particleSystem;
    HueCycleEmitter*    primaryEmitter;

    

};


#endif
