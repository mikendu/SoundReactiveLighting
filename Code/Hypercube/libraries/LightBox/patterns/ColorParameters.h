#ifndef COLOR_PARAMETERS_H
#define COLOR_PARAMETERS_H


#include "Common.h"
#include "SpectrumAnalyzer.h"
#include "HsvOscillator.h"

enum GlobalColorType
{
    PSYCHO,
};

class ColorParameters
{

public:

    ColorParameters()
    {
        float twoPI = 2.0f * M_PI;
        crazyOscillator = new HsvOscillator( { twoPI }, { twoPI / 6.3f }, { twoPI / 7.6f });
    }

    void updateSoundParameters(SpectrumAnalyzer& spectrum)
    {
        float energy = spectrum.getEnergy(SoundBand::BRIGHTNESS) * spectrum.getEnergy(SoundBand::KICK);
        uint16_t crazyEnergy = round(1024.0f * energy);

        crazyScale = min(crazyEnergy, 255);
        oscillatorSpeed = lerp(4.0f, 25.0f, energy);
    }

    void update(float deltaTime)
    {        
        crazyOscillator->setSpeed(oscillatorSpeed);
        crazyOscillator->update(deltaTime);

        crazyColor = crazyOscillator->color();
        crazyColor = scaleColor(crazyColor, crazyScale);
    }

    CRGB getScaledColor(GlobalColorType colorType, uint8_t scale)
    {
        CRGB color = CRGB::Black;
        switch (colorType)
        {
            case PSYCHO:
                color = crazyColor;
                break;

            default:
                break;
        }

        return scaleColor(color, scale);
    }

private:
    
    HsvOscillator* crazyOscillator;
    CRGB crazyColor;
    uint8_t crazyScale;
    float oscillatorSpeed;


};


#endif