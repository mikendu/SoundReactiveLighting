#ifndef BEAT_ANALYZER_H
#define BEAT_ANALYZER_H

#include "Constants.h"
#include "Bundle.h"
#include "LightUtils.h"
#include "RingBuffer.h"

class BeatAnalyzer
{

public:

    BeatAnalyzer() : powerBuffer(255)
    {
    }

    void update(float* spectrum, Bundle& peakData)
    {        
        float spectrumPower = LightUtils::weightedSum(spectrum, Constants::FrequencyWeights, 512) 
                                / (Constants::SpectrumSum / 60.0f);
        power = LightUtils::clamp((spectrumPower + peakData.value3) / 2.0f, 0.0f, 1.0f);
        /*powerBuffer.push(power);
        minPower = LightUtils::rollingAverage(minPower, getMinPower(), 0.001f);*/
        minPower = 0.0f;

        float bassSum = LightUtils::sum(spectrum, 4, 1) / 0.8f;
        bass = LightUtils::clamp(bassSum * bassSum, 0.0f, 1.0f);

        float highSum = LightUtils::sum(spectrum, 400, 64) / 2.0f;
        high = LightUtils::clamp(highSum * highSum, 0.0f, 1.0f);


        // Kick
        float lo = getKickLo(spectrum);
        float hi = getKickHi(spectrum);

        kick = 0.0f;

        if( (hi - 0.01f) > (1.4f * kickHi) && (hi - kickHi) > 0.03f && hi > 0.02f)
            impactHi = MillisecondTimer::timestamp();
        
        
        if((lo - kickLo) > 0.0987f && lo > 0.165f && (lo - 0.05f) > (1.5f * kickLo))
        {
            unsigned long currentTime = MillisecondTimer::timestamp();
            if(currentTime >= impactHi && (currentTime - impactHi) <= 50000)
            {
                kick = LightUtils::clamp(pow(lo, 1.25f) * 1.75f, 0.0f, 1.0f);
            }
        }

        kickLo = LightUtils::rollingAverage(kickLo, lo, 0.1f);
        kickHi = LightUtils::rollingAverage(kickHi, hi, 0.1f);

        float bassI = spectrum[1];
        float bassII = spectrum[2];
        float bassIII = spectrum[3];
        float bassIV = spectrum[4];

        float maxBass = max(max(max(bassI, bassII), bassIII), bassIV);
        if(maxBass == bassI) bassMedian = 0.0f;
        if(maxBass == bassII) bassMedian = 0.3333f;
        if(maxBass == bassIII) bassMedian = 0.667f;
        if(maxBass == bassIV) bassMedian = 1.0f;
    }

    float bassCenter()
    {
        return bassMedian;
    }

    float getKickLo(float* spectrum)
    {
        return LightUtils::sum(spectrum, 3, 2);
    }

    float getKickHi(float* spectrum)
    {
        return LightUtils::sum(spectrum, 12, 6);        
    }

    float bassPower() { return bass; }
    float highPower() { return high; }
    float kickPower() { return kick; }
    float weightedPower() { return power; }
    float powerFloor() { return minPower; }
    float getMinPower() 
    {
        float minPower = 1000.0f;
        for(uint8_t i = 0; i < powerBuffer.size(); i++)
            minPower = min(minPower, powerBuffer.get(i));

        return minPower;
    }


private:

    float bass;
    float high;
    float kick;
    float power;
    float minPower;
    float bassMedian;

    float kickLo;
    float kickHi;

    unsigned long impactLo;
    unsigned long impactHi;
    
    RingBuffer<float> powerBuffer;

};

#endif