#ifndef COMMON_HEADERS_H
#define COMMON_HEADERS_H


#include <FastLED.h>
#include <ILI9341_t3.h>
#include <font_ShareTechMono-Regular.h> // from ILI9341_t3
#if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#endif