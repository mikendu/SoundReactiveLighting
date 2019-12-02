#include "AnalogLED.h"

AnalogLED::AnalogLED( 	int redPin, 
                        int greenPin, 
                        int bluePin, 
                        const float ditherTime  = 5.0f,
                        const CRGB& correction  = TypicalSMD5050, 
                        const CRGB& temperature = Halogen, 
                        const CRGB& balance = Tungsten100W)  :		    pin_r(redPin),
                                                                        pin_g(greenPin),
                                                                        pin_b(bluePin),
                                                                        ditherWindow(ditherTime),
                                                                        ditherCounter(0.0f),
                                                                        flickerCounter(0.0f),
                                                                        flickerValue(true)
{
    this->brightness = 255;
    this->colorCorrection = CRGB(correction);
    this->colorTemperature = CRGB(temperature);
    this->whiteBalance = CRGB(balance);

    this->correctionInterpolation = 0;
    this->rgbColor = CRGB();
}

void AnalogLED::setup()
{	
    pinMode(pin_r, OUTPUT);
    pinMode(pin_g, OUTPUT);
    pinMode(pin_b, OUTPUT);
}

void AnalogLED::show(const CHSV& color, const float brightness)
{
    this->correctionInterpolation = 255 - color.sat;
    hsv2rgb_rainbow(color, this->rgbColor);
    this->show(this->rgbColor, brightness);
}

void AnalogLED::show(const CRGB& color, const float brightness)
{
    this->setBrightness(brightness);
    this->show(color);
}

void AnalogLED::setBrightness(const float brightness)
{
    this->brightness = brightness;
}

void AnalogLED::show(const CRGB& color)
{    
    this->stepDithering();

    CRGB colorTemp = blend(this->colorTemperature, this->whiteBalance, this->correctionInterpolation);
    CRGB adjustment = CLEDController::computeAdjustment(255, this->colorCorrection, colorTemp);

    uint8_t adjustedR = scale8(color.red, adjustment.red);
    uint8_t adjustedG = scale8(color.green, adjustment.green);
    uint8_t adjustedB = scale8(color.blue, adjustment.blue);

    analogWrite(pin_r, this->dither(adjustedR));
    analogWrite(pin_g, this->dither(adjustedG));
    analogWrite(pin_b, this->dither(adjustedB));
}



void AnalogLED::stepDithering()
{
    this->ditherCounter += MillisecondTimer::elapsed();
    if(this->ditherCounter > this->ditherWindow)
        this->ditherCounter = 0.0f;
}
    
uint8_t AnalogLED::dither(uint8_t color)
{
    float scaled = color * this->brightness;
    uint8_t top = (uint8_t)ceil(scaled);
    uint8_t bottom = (uint8_t)floor(scaled);
    
    float percentage = scaled - bottom;
    float threshold = this->ditherWindow * percentage;
    return (this->ditherCounter < threshold) ? top : bottom;
}

CHSV AnalogLED::flicker(CHSV input, float flickerFrequency)
{
    this->flickerCounter += MillisecondTimer::elapsed();
    float period = (1.0f / flickerFrequency) * 1000.0f;
    
    if (flickerCounter >= period)
    {
        flickerCounter -= period;
        flickerValue = !flickerValue;
    }

    return flickerValue ? input : CHSV(0, 0, 0);
    
}