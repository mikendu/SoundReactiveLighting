#include "CustomDisplay.h"


CustomDisplay::CustomDisplay() : CustomDisplay(TFT_CS, TFT_DC, TFT_RST, TFT_MOSI, TFT_SCLK, TFT_MISO)
{
}

CustomDisplay::CustomDisplay(   byte csPin, 
                                byte dcPin,    
                                byte resetPin, 
                                byte mosiPin, 
                                byte sclPin, 
                                byte misoPin) : display(nullptr)
{
    this->display = new ILI9341_t3(csPin, dcPin, resetPin, mosiPin, sclPin, misoPin);
}

void CustomDisplay::initialize()
{
    this->display->begin();
    this->display->setFont(ShareTechMono_12);
    this->display->setTextColor(ILI9341_WHITE);
    this->display->setRotation(1);
    this->clear();
}

void CustomDisplay::clear()
{    
    this->display->fillScreen(ILI9341_BLACK);
}

void CustomDisplay::drawTitle(const char* text)
{
    this->display->fillRect(101, 40, 118, 15, ILI9341_BLACK);
    
    uint16_t width = this->display->measureTextWidth(text);
    uint16_t x = (320 - width) / 2;
    
    this->display->setCursor(x, 40);
    this->display->print(text);

}

void CustomDisplay::drawPeak(float value, uint16_t x, uint16_t y, uint16_t width, uint16_t maxHeight)
{
    uint16_t height = value * (maxHeight - 2);
    uint16_t remainder = maxHeight - height;
    uint16_t topColor = CL(104, 230, 25);
    uint16_t bottomColor = CL(24, 230, 21);
    uint16_t remainderColor = (value >= 1.0f) ? ILI9341_RED : ILI9341_BLACK;

    
    this->display->fillRect(x, y, width, remainder, remainderColor);
    this->display->fillRectVGradient(x, y + remainder, width, height, bottomColor, topColor);

}

void CustomDisplay::drawEQ(uint8_t index, float value, uint16_t x, uint16_t y, uint16_t width, uint16_t maxHeight)
{
    uint16_t height = value * maxHeight;
    uint16_t remainder = maxHeight - height;
    uint16_t topColor = CL(0, 203, 230);
    uint16_t bottomColor = CL(2, 128, 186);
    
    if (value < 1.0f)
        this->display->fillRect(x, y, width, remainder, ILI9341_BLACK);

    this->display->fillRectVGradient(x, y + remainder, width, height, topColor, bottomColor);

}

void CustomDisplay::drawAudio(  float* data, byte dataLen, 
                                float peakLeft, float peakRight, float peakMiddle)
{
    uint16_t barHeight = 80;
    uint16_t top = 100;
    uint8_t len = min(dataLen, 16);

    // Peak Bars
    this->drawPeak(peakLeft, 70, top, 7, barHeight);
    this->drawPeak(peakRight, 80, top, 7, barHeight);


    for(uint8_t i = 0; i < len; i++) 
    {
        float clamped = min(data[i] * 1.85f, 1.0f);
        uint16_t x = 150 + (7 * i);
        this->drawEQ(i, clamped, x, top, 4, barHeight);
        
    }
}

void CustomDisplay::drawSlider(float value, bool color = false)
{
    float scaledValue = value * 10.0f;
    for (uint8_t i = 0; i < 10; i++)
    {
        uint16_t x = 90 + (i * 14);
        uint16_t height = 10 * (i + 1);
        uint16_t y = 70 + (100 - height);

        uint16_t topColor = !color ? CL(230, 42, 0) : CL(177, 181, 180);
        uint16_t bottomColor = !color ? CL(255, 98, 0) : CL(235, 240, 238);

        uint16_t scaledColorTop = this->scaleColor(i, topColor, scaledValue);
        uint16_t scaledColorBottom = this->scaleColor(i, bottomColor, scaledValue);

        this->display->fillRectVGradient(x, y, 10, height, scaledColorTop, scaledColorBottom);
    }
}

uint16_t CustomDisplay::scaleColor(uint8_t index, uint16_t inputColor, float scaledValue)
{
    float diff = max(0.0f, min(scaledValue - index, 1.0f));
    uint8_t r, g, b;

    ILI9341_t3::color565toRGB(inputColor, r, g, b);
    r = (uint8_t)(r * diff);
    g = (uint8_t)(g * diff);
    b = (uint8_t)(b * diff);

    return CL(r, g, b);
}


void CustomDisplay::sliderText(const char* text)
{       
    uint16_t width = this->display->measureTextWidth(text);
    uint16_t height = this->display->measureTextHeight(text);
    uint16_t x = (320 - width) / 2;
    
    this->display->setCursor(x, 200);
    this->display->print(text);
}

void CustomDisplay::debugText(const char* text)
{       
    this->display->fillRect(100, 190, 120, 30, ILI9341_BLACK);
    uint16_t width = this->display->measureTextWidth(text);
    uint16_t x = (320 - width) / 2;
    
    this->display->setCursor(x, 200);
    this->display->print(text);
}