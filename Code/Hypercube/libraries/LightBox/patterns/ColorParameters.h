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
        float energy = pow(spectrum.getEnergy(SoundBand::BRIGHTNESS), 1.25f);
        uint16_t crazyEnergy = round(300 * energy);

        crazyScale = min(crazyEnergy, 255);
        oscillatorSpeed = lerp(5.0f, 10.0f, energy);
    }

    void update(float deltaTime)
    {        
        crazyOscillator->setSpeed(oscillatorSpeed);
        crazyOscillator->update(deltaTime);

        crazyColor = crazyOscillator->color();
        scaleColor(crazyColor, crazyScale);
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

        scaleColor(color, scale);
        return color;
    }

private:
    
    HsvOscillator* crazyOscillator;
    CRGB crazyColor;
    uint8_t crazyScale;
    float oscillatorSpeed;


};


#endif