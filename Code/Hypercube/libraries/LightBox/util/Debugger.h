#ifndef DEBUGGER_H
#define DEBUGGER_H


#include "Common.h"

static inline void print(const float value, const float scalar, const char* name, bool end = false)
{
    Serial.print(name);
    Serial.print(":");
    float scaled = scalar * value;
    if (end) 
    {
        Serial.println(scaled);
    }
    else
    {
        Serial.print(scaled);
        Serial.print(",");
    }
}

static inline void println(const float value, const float scalar, const char* name)
{
    print(value, scalar, name, true);
}

#endif