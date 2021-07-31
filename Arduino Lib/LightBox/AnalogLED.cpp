#include "AnalogLED.h"

AnalogLED::AnalogLED( 	MillisecondTimer& timerRef,
                        int redPin, 
                        int greenPin, 
                        int bluePin, 
                        const float ditherTime  = 5.0f,
                        const CRGB& correction  = TypicalSMD5050, 
                        const CRGB& temperature = Halogen, 
                        const CRGB& balance = Tungsten100W)  :		    timer(timerRef),
                                                                        pin_r(redPin),
                                                                        pin_g(greenPin),
                                                                        pin_b(bluePin),
                                                                        ditherWindow(ditherTime),
                                                                        ditherCounter(0.0f),
                                                                        flickerCounter(0.0f),
                                                                        flickerValue(true)
{
    this->brightness = 1.0f;
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

byte test = 0;
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
    
    analogWrite(pin_r, this->dither(adjustedR, true));
    analogWrite(pin_g, this->dither(adjustedG, false));
    analogWrite(pin_b, this->dither(adjustedB, false));


    /*
    uint8_t scale = (uint8_t) round(255 * this->brightness);
    adjustedR = scale8(adjustedR, scale);
    adjustedG = scale8(adjustedG, scale);
    adjustedB = scale8(adjustedB, scale);
    analogWrite(pin_r, adjustedR);
    analogWrite(pin_g, adjustedG);
    analogWrite(pin_b, adjustedB);*/
}



void AnalogLED::stepDithering()
{
    this->ditherCounter += this->timer.elapsed();
    if(this->ditherCounter > this->ditherWindow)
        this->ditherCounter = 0.0f;
}
    
uint8_t AnalogLED::dither(uint8_t color, bool print)
{
    float scaled = color * this->brightness;

    test += 1;
    if (test >= 255 && print) {
        test = 0;
        Serial.print("Brightness: ");
        Serial.print(this->brightness, 3);
        Serial.print(", unscaled: ");
        Serial.print(color);
        Serial.print(", scaled: ");
        Serial.print(scaled);
        Serial.println("");
    }

    uint8_t top = (uint8_t)ceil(scaled);
    uint8_t bottom = (uint8_t)floor(scaled);
    
    float percentage = scaled - bottom;
    float threshold = this->ditherWindow * percentage;
    return (this->ditherCounter < threshold) ? top : bottom;
}

CHSV AnalogLED::flicker(CHSV input, float flickerFrequency)
{
    this->flickerCounter += this->timer.elapsed();
    float period = (1.0f / flickerFrequency) * 1000.0f;
    
    if (flickerCounter >= period)
    {
        flickerCounter -= period;
        flickerValue = !flickerValue;
    }

    return flickerValue ? input : CHSV(input.hue, input.sat, 0);
    
}