#ifndef MATH_UTILS_H
#define MATH_UTILS_H


static inline float wrap(const float value, const float max)
{
    return (value >= max) ? 0.0f : value;
}

// static inline float randomFloat(const int resolution = 1024)
// {
//     int randomInt = random(1, resolution + 1);
//     return ((float)randomInt) / ((float)resolution);
// }

static inline float lerp(const float minVal, const float maxVal, const float value)
{
    float diff = maxVal - minVal;
    float val = min(1.0f, max(0.0f, value));
    return minVal + (diff * val);
}

static inline float clamp(const float value, const float minVal, const float maxVal)
{
    return max(minVal, min(maxVal, value));
}

static inline float sum(const float* data, const uint16_t len, const uint16_t start = 0)
{
    float result = 0.0f;
    for(uint16_t i = start; i < len; i++)
        result += data[i];

    return result;
}

static inline float weightedSum(const float* data, const float* weights, 
                                const uint16_t len, const uint16_t start = 0)
{
    float result = 0.0f;
    for(uint16_t i = start; i < len; i++)
        result += (data[i] * weights[i]);

    return result;
}

static inline float indexWeightedSum(const float* data, const uint16_t len, const uint16_t start = 0)
{
    float result = 0.0f;
    for(uint16_t i = start; i < len; i++)
        result += (data[i] * ((3 * i) + 1));

    return result;
}

static inline float rollingAverage(const float current, const float newValue, const float alpha)
{
    return (current * (1.0f - alpha)) + (newValue * alpha);
}    

static inline CHSV blend(CHSV colorOne, CHSV colorTwo, const float blendAmt)
{
    uint8_t hue = (uint8_t)round(lerp(colorOne.hue, colorTwo.hue, blendAmt));
    uint8_t sat = (uint8_t)round(lerp(colorOne.sat, colorTwo.sat, blendAmt));
    uint8_t val = (uint8_t)round(lerp(colorOne.val, colorTwo.val, blendAmt));

    return CHSV(hue, sat, val);
}

static inline uint8_t mod(signed char value, const uint8_t limit) 
{
    if (value < 0) 
    {
        value += limit;
    }
    return value % limit;
}


static inline int floatComparison(const void* a, const void* b) 
{
  if ( *(float*)a <  *(float*)b ) return -1;
  if ( *(float*)a == *(float*)b ) return 0;
  if ( *(float*)a >  *(float*)b ) return 1;
}


static inline float randomFloat() 
{
    return random16() / 65535.0f;
}

static inline uint8_t random(uint8_t min, uint8_t max)
{
    uint8_t range = max - min;
    return qadd8(scale8(random8(), range), min);
}

static inline float random(float min, float max)
{
    float range = max - min;
    return ((randomFloat() * range) + min);
}

static inline void scaleColor(CRGB& color, const uint8_t scale)
{
    color.r = scale8(color.r, scale);
    color.g = scale8(color.g, scale);
    color.b = scale8(color.b, scale);
}

#endif