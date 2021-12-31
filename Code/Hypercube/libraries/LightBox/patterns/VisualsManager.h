#ifndef PATTERN_MANAGER_H
#define PATTERN_MANAGER_H

#include "ParticleSystem.h"
#include "HueCycleEmitter.h"
#include "ColorParameters.h"
#include "BeatVisualizer.h"
#include "MillisecondTimer.h"
#include "Common.h"

class VisualsManager
{
public:

    VisualsManager(SpectrumAnalyzer& analyzer, NeopixelStrip& ledStrip) : spectrum(analyzer), strip(ledStrip)
    {
        timer = new MillisecondTimer(true);
        timer->start();
        
        colorOscillator = new HsvOscillator( { 67.2f }, { 5.75f, 0.3625f, 0.6875f }, { 7.35f, 0.275f, 0.475f });
        colorOscillator->setFlutter({ 9.0f, 0.4f, 0.6f});

        analyzer.addCallback(std::bind(&VisualsManager::updateSoundParameters, this));
        // particleSystem.addEmitter(new HueCycleEmitter(1.0f, 0.0f, 0, colorOscillator));

        particleSystem.addEmitter(new HueCycleEmitter(1.0f / 3.0f, 0.0f, -16, colorOscillator, 60, LED_COUNT));
        particleSystem.addEmitter(new HueCycleEmitter(1.0f / 3.0f, 1.0f, 0, colorOscillator, 60, LED_COUNT));
        particleSystem.addEmitter(new HueCycleEmitter(1.0f / 3.0f, 2.0f, -16, colorOscillator, 60, LED_COUNT));

        particleSystem.addEmitter(new HueCycleEmitter(1.0f / 4.0f, 0.0f, -16, colorOscillator, 0, 60));
        particleSystem.addEmitter(new HueCycleEmitter(1.0f / 4.0f, 1.0f, 0, colorOscillator, 0, 60));
        particleSystem.addEmitter(new HueCycleEmitter(1.0f / 4.0f, 2.0f, -16, colorOscillator, 0, 60));

        // particleSystem.addEmitter(new HueCycleEmitter(1.0f / 3.0f, 0.0f, -16, colorOscillator));
        // particleSystem.addEmitter(new HueCycleEmitter(1.0f / 3.0f, 1.0f, 0, colorOscillator));
        // particleSystem.addEmitter(new HueCycleEmitter(1.0f / 3.0f, 2.0f, -16, colorOscillator));
    }

    void updateSoundParameters()
    {
        derivedParameters.compute(spectrum);
        beatVisualizer.updateSoundParameters(spectrum);
        colorParameters.updateSoundParameters(spectrum);
        particleSystem.updateSoundParameters(spectrum, derivedParameters);
        colorOscillator->setSpeed(derivedParameters.getParameter(DerivedParameterType::OSCILLATOR_SPEED));        
    }

    void update()
    {
        float deltaTime = timer->update() / 1000.0f;
        
        colorOscillator->update(deltaTime);
        colorParameters.update(deltaTime);

        particleSystem.update(deltaTime);
        particleSystem.show(strip, colorParameters);
        beatVisualizer.show(strip);
    }


private:

    MillisecondTimer*   timer;
    SpectrumAnalyzer&   spectrum;
    NeopixelStrip&      strip;
    DerivedParameters   derivedParameters;
    ColorParameters     colorParameters;
    
    // Visuals
    HsvOscillator*      colorOscillator;
    ParticleSystem      particleSystem;
    BeatVisualizer      beatVisualizer;

    

};


#endif

