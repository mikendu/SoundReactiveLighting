#ifndef LIGHT_PARTICLE_H
#define LIGHT_PARTICLE_H


/**
 * Properties:
 * - Size
 * - Size over life
 * - Velocity (speed + direction)
 * - Brightness
 * - Brightness over life (fade in/out always? maybe just control fade curve)?
 * - Hue
 * - Hue over life
 * - Saturation
 * - Saturation over life
 * - Lifetime/duration
 * - Spread curve (sharpness/blur)
 * - Size by intensity
 * - Brightness by intensity
 * - Saturation by intensity
 * - Hue by intensity
 */

/**
 * Emitter Properties:
 * - Emission rate over time (sine wave)
 * - Emission rate by intensity
 * - Initial size by intensity/sound brightness
 * - Initial saturation/brightness by intensity
 * - Pool Size
 */

#include "NeopixelStrip.h"
#include "SpectrumAnalyzer.h"
#include "DerivedParameters.h"
#include "ColorParameters.h"

struct ParticleProperties
{
    float lifespan = 1.0f;
    uint8_t hue = 255;
    uint8_t saturation = 255;
    uint8_t value = 255;
    float size = 5.0f;
    float velocity = 5.0f;
    float hardness = 1.0f;
    // controls "ramp up/down" of brightness. 0 = no ramp, 1 = full curve up and down
    float brightnessProfile = 0.5f; 

    float minLocation = 0.0f;
    float maxLocation = LED_COUNT;
};

class LightParticle
{

public:
    LightParticle() : progress(1.0f), life(0.0f), progress8(255)
    {
    }

    void spawn(ParticleProperties properties, float location) 
    {
        progress = 0.0f;
        life = 0.0f;
        this->location = location;
        this->properties = properties;

        baseRadius = properties.size / 2.0f; 
        radius = properties.size / 2.0f;
        hardnessFactor = 255 * properties.hardness;
        baseHardness = hardnessFactor;

        profileFactor = lerp(4096, 256, pow(properties.brightnessProfile, 0.5f));
        satRange = 255 - properties.saturation;
        valRange = 255 - properties.value;
        locationRange = properties.maxLocation - properties.minLocation;
    }

    void updateSoundParameters(SpectrumAnalyzer& spectrum, DerivedParameters& parameters)
    {
        if (!isAlive())
            return;
        
        #ifdef SOUND_REACTIVE
            uint8_t satIncrease = satRange * spectrum.getIntensity();
            uint8_t valIncrease = valRange * spectrum.getIntensity();
            currentSat = qadd8(properties.saturation, satIncrease);
            currentVal = qadd8(properties.value, valIncrease);
        #else
            currentSat = properties.saturation;
            currentVal = properties.value;
        #endif
        
        float intensity = parameters.getParameter(DerivedParameterType::PARTICLE_INTENSITY);
        radius = baseRadius * intensity;
        hardnessFactor = baseHardness * intensity;
        speed = parameters.getParameter(DerivedParameterType::PARTICLE_SPEED); 
    }
    
    bool update(float elapsedSeconds) 
    {
        if (!isAlive())
            return false;

        life += elapsedSeconds;
        progress = min(life / properties.lifespan, 1.0f);
        progress8 = round(255 * progress);

        // Location
        location += (properties.velocity * elapsedSeconds * speed);
        while (location > properties.maxLocation)
            location -= locationRange;

        while (location < properties.minLocation)
            location += locationRange;


        // Brightness
        if (properties.brightnessProfile == 0.0f) 
        {
            brightness = 255;
        } 
        else 
        {           
            float midLifeDistance = constrain(1.0f - (abs(0.5f - progress) / 0.5f), 0.0f, 1.0f);
            uint8_t brightnessInput = min(round(profileFactor * midLifeDistance), 255);
            brightness = ease8InOutCubic(brightnessInput);
        }

        return isAlive();
    }

    void show(NeopixelStrip& strip, ColorParameters& parameters) 
    {
        if (!isAlive())
            return;

        uint16_t stripLen = locationRange;
        short startPixel = round(location - radius) + stripLen;
        short endPixel = round(location + radius) + stripLen;

        for (short i = startPixel; i <= endPixel; i++) 
        {
            uint16_t locationIndex = i - stripLen;
            float distance = constrain(1.0f - (abs(location - locationIndex) / radius), 0.0f, 1.0f);
            uint8_t value = min(round(hardnessFactor * distance), 255);
            value = ease8InOutCubic(value);

            uint8_t scaledBrightness = scale8(scale8(currentVal, value), brightness);
            uint16_t index = (i  % stripLen) + properties.minLocation;
            CRGB mainColor = CHSV(properties.hue, currentSat, scaledBrightness);
            CRGB secondaryColor = parameters.getScaledColor(GlobalColorType::PSYCHO, scaledBrightness);
            strip[index] += mainColor;
            strip[index] += secondaryColor;
        }
    }

    bool isAlive() 
    {
        return progress < 1.0f;
    }


private:

    uint8_t progress8;
    float progress;
    float life;
    float location;
    ParticleProperties properties;

    uint8_t brightness;
    uint8_t satRange;
    uint8_t valRange;

    uint8_t currentSat;
    uint8_t currentVal;

    float baseHardness;
    float hardnessFactor;
    float profileFactor;

    float radius;
    float baseRadius;
    float speed;
    float locationRange;
};




#endif