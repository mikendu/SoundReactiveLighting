#ifndef GAIN_CALIBRATOR_H
#define GAIN_CALIBRATOR_H

#include "Common.h"


class GainCalibrator
{
public:
    GainCalibrator(AudioAnalyzePeak& audioIn, NeopixelStrip& strip, uint8_t count): audio(audioIn), 
                                                                                    ledStrip(strip), 
                                                                                    ledCount(count),
                                                                                    audioLevel(0.0f)
    {
    }

    void update() 
    {
            CRGB* leds = ledStrip.getLeds();
            audioLevel = audio.available() ? audio.read() : audioLevel;
            // linearLevel = pow(audioLevel, 10);
            // Serial.println(linearLevel);
            
            memset8( leds, 0, ledCount * sizeof(CRGB));
            float ledLevel = audioLevel * ledCount;

            for(int i = 0; i < ledCount; i++) 
            {
                float pixelValue = constrain(ledLevel - i, 0.0f, 1.0f);
                uint8_t brightness = round(255 * pixelValue);      
                leds[i] = CRGB(brightness, brightness, brightness);
            }

            ledStrip.show();
    }

private:
    AudioAnalyzePeak&       audio;
    uint8_t                 ledCount;
    NeopixelStrip&          ledStrip;
    float                   audioLevel;


};


#endif