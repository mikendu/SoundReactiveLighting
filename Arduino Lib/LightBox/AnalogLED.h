#ifndef ANALOG_LED_H
#define ANALOG_LED_H


#include "CommonHeaders.h"
#include "MillisecondTimer.h"


#define PIN_R 9
#define PIN_G 10
#define PIN_B 11

class AnalogLED
{

private: 

    const int pin_r;
    const int pin_g;
    const int pin_b;

    float brightness;
    uint8_t correctionInterpolation;
    const CRGB& colorCorrection;
    const CRGB& colorTemperature;
    const CRGB& whiteBalance;    
    CRGB rgbColor;

    float ditherWindow;
    float ditherCounter;
    float flickerCounter;
    bool flickerValue;

    void stepDithering();
    uint8_t dither(uint8_t color);

public:

    AnalogLED( 	int redPin, 
                int greenPin, 
                int bluePin, 
                const float ditherTimer = 5.0f,
                const CRGB& correction  = TypicalSMD5050, 
                const CRGB& temperature = Halogen, 
                const CRGB& balance = Tungsten100W);

    void setup();
    void show(const CRGB& color);
    void show(const CRGB& color, const float brightness);
    void show(const CHSV& color, const float brightness);
    void setBrightness(const float brightness);

    CHSV flicker(CHSV input, float flickerFrequency);
};



#endif