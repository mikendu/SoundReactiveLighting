#ifndef NEOPIXEL_STRIP_H
#define NEOPIXEL_STRIP_H

#define MAX_BRIGHTNESS 225
#define NEOPIXEL_PIN 1

#include "Common.h"

class NeopixelStrip 
{

public:
    static void             delay(int milliseconds);
    NeopixelStrip();

    void                    initialize(uint16_t length, uint8_t initialBrightness = 0);
    CRGB*                   getLeds(uint16_t offset = 0);
    uint16_t                length();

    void                    setBrightness(uint8_t brightness);
    void                    show();

    void                    fill(CRGB color, uint16_t count = -1, uint16_t start = 0);

    void                    clear(uint16_t count = -1, uint16_t start = 0);
    void                    clearNow(uint16_t count = -1, uint16_t start = 0);


    // Operators
    CRGB                    operator[](uint16_t i) const;
    CRGB&                   operator[](uint16_t);

private:

    CRGB*                   leds;
    uint16_t                totalLength;
};



#endif