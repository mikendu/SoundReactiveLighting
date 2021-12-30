#include "NeopixelStrip.h"

/* -- Static -- */


void NeopixelStrip::delay(int milliseconds)
{
    FastLED.delay(milliseconds);
}


/* -- End Static -- */

NeopixelStrip::NeopixelStrip(): totalLength(0), leds(nullptr)
{
}

void NeopixelStrip::initialize(uint16_t length, uint8_t initialBrightness = 0)
{
    leds = new CRGB[length];
    totalLength = length;    
	LEDS.addLeds<WS2812SERIAL, NEOPIXEL_PIN, BRG>(leds, length);

    // Correction options: TypicalSMD5050, UncorrectedColor
    // Temperature options: Tungsten40W, Tungsten100W, Halogen, CarbonArc, HighNoonSun, DirectSunlight, UncorrectedTemperature 
    FastLED.setCorrection(TypicalSMD5050);
    FastLED.setTemperature(CarbonArc);
    FastLED.setDither(BINARY_DITHER);
    FastLED.clear();

    setBrightness(initialBrightness);
}

void NeopixelStrip::show()
{
    FastLED.show();
}

void NeopixelStrip::setBrightness(uint8_t value)
{
    uint8_t brightness = scale8(dim8_raw(value), MAX_BRIGHTNESS);
    FastLED.setBrightness(brightness);
}

void NeopixelStrip::fill(CRGB color, uint16_t count = -1, uint16_t start = 0)
{
    if (count < 0 || count > totalLength) 
        count = totalLength;

    uint16_t end = start + count;
    for(uint16_t i = start; i < end; i++) 
        leds[i] = color;
}


CRGB* NeopixelStrip::getLeds(uint16_t offset = 0)
{
    return &leds[offset];
}

uint16_t NeopixelStrip::length()
{
    return totalLength;
}

void NeopixelStrip::clear(uint16_t count = -1, uint16_t start = 0) 
{
    if (count < 0 || count > totalLength) 
        count = totalLength;

    count -= start;
    memset8( leds, start, count * sizeof(CRGB));
}

void NeopixelStrip::clearNow(uint16_t count = -1, uint16_t start = 0) 
{
    clear(count, start);
    show();
}


// Operators

CRGB NeopixelStrip::operator[](uint16_t i) const 
{   
    return leds[i];
}

CRGB& NeopixelStrip::operator[](uint16_t i) 
{
    return leds[i];
}