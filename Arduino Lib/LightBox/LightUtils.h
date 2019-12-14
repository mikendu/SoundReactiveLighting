#ifndef LIGHT_UTILS_H
#define LIGHT_UTILS_H

#include "CommonHeaders.h"

class LightUtils
{
public:

    static float wrap(float value, float max)
    {
        return (value >= max) ? 0.0f : value;
    }

    static float randomFloat(int resolution = 1024)
    {
        int randomInt = random(1, resolution + 1);
        return ((float)randomInt) / ((float)resolution);
    }

    static float lerp(float minVal, float maxVal, float value)
    {
        float diff = maxVal - minVal;
        float val = min(1.0f, max(0.0f, value));
        return minVal + (diff * val);
    }

    static float clamp(float value, float minVal, float maxVal)
    {
        return max(minVal, min(maxVal, value));
    }

    static float sum(float* data, uint16_t len, uint16_t start = 0)
    {
        float result = 0.0f;
        for(uint16_t i = start; i < len; i++)
            result += data[i];

        return result;
    }

    static float weightedSum(float* data, float* weights, uint16_t len, uint16_t start = 0)
    {
        float result = 0.0f;
        for(uint16_t i = start; i < len; i++)
            result += (data[i] * weights[i]);

        return result;
    }

    static float indexWeightedSum(float* data, uint16_t len, uint16_t start = 0)
    {
        float result = 0.0f;
        for(uint16_t i = start; i < len; i++)
            result += (data[i] * ((3 * i) + 1));

        return result;
    }

    static float rollingAverage(float current, float newValue, float alpha)
    {
        return (current * (1.0f - alpha)) + (newValue * alpha);
    }    

    static CHSV blend(CHSV colorOne, CHSV colorTwo, float blendAmt)
    {
        uint8_t hue = (uint8_t)round(lerp(colorOne.hue, colorTwo.hue, blendAmt));
        uint8_t sat = (uint8_t)round(lerp(colorOne.sat, colorTwo.sat, blendAmt));
        uint8_t val = (uint8_t)round(lerp(colorOne.val, colorTwo.val, blendAmt));

        return CHSV(hue, sat, val);
    }
};

#endif