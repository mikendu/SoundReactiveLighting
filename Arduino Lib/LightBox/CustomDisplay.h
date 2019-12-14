#ifndef CUSTOM_DISPLAY_H
#define CUSTOM_DISPLAY_H

#include "CommonHeaders.h"
#include "ColorTheme.h"

#define TFT_RST    	14
#define TFT_DC     	9
#define TFT_CS    	10
#define TFT_MOSI  	11
#define TFT_MISO  	12
#define TFT_SCLK  	13


class CustomDisplay 
{

public:
    CustomDisplay();
    CustomDisplay(  byte csPin, 
                    byte dcPin,    
                    byte resetPin, 
                    byte mosiPin, 
                    byte sclPin, 
                    byte misoPin);

    void initialize();
    void clear();
    void drawTitle(const char* text);
    void drawAudio( float* data, byte dataLen, 
                    float peakLeft, float peakRight, float peakMiddle);
    void drawSlider(float value, bool color = false);    
    void sliderText(const char* text);
    void debugText(const char* text);

private:

    void drawPeak(float value, uint16_t x, uint16_t y, uint16_t width, uint16_t maxHeight);
    void drawEQ(uint8_t index, float value, uint16_t x, uint16_t y, uint16_t width, uint16_t maxHeight);
    uint16_t scaleColor(uint8_t index, uint16_t inputColor, float scaledValue);

    ILI9341_t3* display;
    

};


#endif