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
        // todo scale with square of brightness energy
        // todo scale beyond 255
        // todo scale oscillator speed by brightness energy
        crazyScale = round(255 * spectrum.getEnergy(SoundBand::BRIGHTNESS));
    }

    void update(float deltaTime)
    {        
        crazyOscillator->setSpeed(10.0f);
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


};


#endif