void loop() 
{    
#ifdef CALIBRATE_GAIN

    ledStrip.setBrightness(brightnessController.update());
    gainCalibrator.update();
    
#else


//    static uint8_t starthue = 0;
//    CRGB* leds = ledStrip.getLeds();
//    fill_rainbow(leds, ledStrip.length(), --starthue, 20);
//    ledStrip.show();


//    fillSnake(ledIndex, ledStrip, CRGB::White);
//    ledStrip.show();
//    
//    NeopixelStrip::delay(1);
//    fillSnake(ledIndex, ledStrip, CRGB::Black);
//    ledIndex = (ledIndex + 1) % ledStrip.length();

    if (analyzer.available()) {
        float val = analyzer.read(0, 127);
        totalIntensity = (0.1f * (val / 3.0f)) + (0.9f * totalIntensity);
    }
    
    CRGB* leds = ledStrip.getLeds();
    memset8( leds, 0, LED_COUNT * sizeof(CRGB));
    float ledLevel = totalIntensity * LED_COUNT;

    for(int i = 0; i < LED_COUNT; i++) 
    {
        float pixelValue = constrain(ledLevel - i, 0.0f, 1.0f);
        uint8_t brightness = round(255 * pixelValue);      
        leds[i] = CRGB(brightness, brightness, brightness);
    }
    ledStrip.show();    
    ledStrip.setBrightness(brightnessController.update());
    
#endif
}

void fillSnake(uint16_t startIndex, NeopixelStrip& strip, CRGB color) {
    CRGB* leds = ledStrip.getLeds();
    for(int i = 0; i < SNAKE_LEN; i++) {
        uint16_t index = (startIndex + i) % ledStrip.length();
        leds[index] = color;
    }
}