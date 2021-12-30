#include "SpectrumAnalyzer.h"

const static float CENTROID_MAX = log(CENTROID_MAX_FREQUENCY);
const static float CENTROID_MIN = log(CENTROID_MIN_FREQUENCY);
const static float CENTROID_DELTA = CENTROID_MAX - CENTROID_MIN;

FrequencyBand BANDS[] = 
{ 
    { 60, 130, 0.0735f },       // Kick: 60 - 130 Hz
    { 345, 750, 0.01425f }      // Snare: 345 - 750 Hz
};


SpectrumAnalyzer::SpectrumAnalyzer( AudioAnalyzeFFT1024& fft, 
                                    AudioAnalyzePeak& peak) :  fourierAnalyzer(fft), peakAnalyzer(peak), intensity(0)
{
    this->frequencyBands = BANDS;
    for (uint8_t i = 0; i < NUM_BANDS; i++) 
    {
        FrequencyBand band = frequencyBands[i];
        frequencyBands[i].binStart = floor(band.minFrequency / FFT_BIN_WIDTH);
        frequencyBands[i].binEnd = floor(band.maxFrequency / FFT_BIN_WIDTH);
        frequencyBands[i].binWidth = (frequencyBands[i].binEnd - frequencyBands[i].binStart) + 1;
        spectrumHistory[i] = RingBuffer<float>(FFT_HISTORY_SIZE); 
        onsets[i] = false;  
    }
}

void SpectrumAnalyzer::update()
{
    if (peakAnalyzer.available()) 
    {
        rawAmplitude = peakAnalyzer.read();
    }

    if (fourierAnalyzer.available())
    {
        for (uint8_t i = 0; i < NUM_BANDS; i++) 
        {
            FrequencyBand band = frequencyBands[i];
            RingBuffer<float>& history = spectrumHistory[i];

            float energy = getInstantaneousEnergy(band.binStart, band.binEnd, band.binWidth);
            float averageEnergy = getAverageEnergy(history);
            float variance = getEnergyVariance(history, averageEnergy);
            float thresholdMultiplier = (-15.0f * variance) + THRESHOLD;
            float threshold = thresholdMultiplier * averageEnergy;
            history.push(energy);  

            bool currentState = (energy - band.minEnergy) > threshold;
            bool lastState = onsetHistory[i];
            onsetHistory[i] = currentState;

            if (currentState == lastState)
                onsetCounters[i] = min(onsetCounters[i] + 1, 10);
            else
                onsetCounters[i] = 0;

            uint8_t onsetThreshold = (currentState ? ONSET_TRIGGER_ON_THRESHOLD : ONSET_TRIGGER_OFF_THRESHOLD);
            if (onsetCounters[i] >= onsetThreshold)
                onsets[i] = currentState;   

            
            if (i == 0) 
                kickEnergy = constrain(energy / 0.25f, 0.0f, 1.0f);
            else
                snareEnergy = constrain(energy / 0.025f, 0.0f, 1.0f);

            // print(threshold, 100, "threshold");
            // print(band.minEnergy, 100, "minEnergy");            
            // print(energy - band.minEnergy, 100, "adjusted");
            // print(energy, 100, "raw");           
        }

        // print(kickEnergy, 10, "kickEnergy");
        // print(snareEnergy, 10, "snareEnergy");
        // print((onsets[0] ? 10 : 0), 1, "kick");
        // println((onsets[1] ? 10 : 0), 1, "snare");

        float bright = 0.0f;
        float weightedFrequencySum = 0.0f;
        float energySum = 0.0f;
        float flux = 0.0f;
        amplitudeSum = 0.0f;
        for (uint16_t i = 0; i < FFT_BIN_COUNT; i++) 
        {
            float amplitude = fourierAnalyzer.read(i);
            float energy = pow(amplitude, 2.0f);
            if (i > 45 && i < 255) 
                bright += amplitude;
            
            flux += pow(amplitude - previousSpectrum[i], 2.0f);
            spectrum[i] = amplitude;
            previousSpectrum[i] = amplitude;
            energySpectrum[i] = energy;            
            weightedFrequencySum += (energy * i);
            energySum += energy;
            amplitudeSum += amplitude;
        }
        flux = constrain(flux / 0.025f, 0.0f, 1.0f);
        amplitudeSum = constrain(amplitudeSum / 1.8f, 0.0f, 1.0f);

        // Centroid
        float centroidBin = (weightedFrequencySum / energySum);
        float centroidFrequency = (centroidBin + 0.5f) * FFT_BIN_WIDTH;
        float centroidValue = log(centroidFrequency + 1.0f);
        float centroid = constrain((centroidValue - CENTROID_MIN) / CENTROID_DELTA, 0.0f, 1.0f);

        // Bandwidth
        float weightedDeviation = 0.0f;
        for (uint16_t i = 0; i < FFT_BIN_COUNT; i++) 
        {
            weightedDeviation += (energySpectrum[i] * pow(i - centroidBin, 2.0f));
        }
        float binBandwidth = sqrt(weightedDeviation / energySum);
        float bandwidth = constrain(binBandwidth / 60.0f, 0.0f, 1.0f);
        
        // Contrast
        // sort ascending, low -> high
        uint16_t contrastBandSize = CONTRAST_SUB_BAND_END - CONTRAST_SUB_BAND_START;
        qsort(&spectrum[CONTRAST_SUB_BAND_START], contrastBandSize, sizeof(float), floatComparison);
        float topAverage = 0.0f;
        float bottomAverage = 0.0f;

        for(uint16_t i = 0; i < CONTRAST_BIN_COUNT; i++) 
        {
            bottomAverage += spectrum[i + CONTRAST_SUB_BAND_START];
            topAverage += spectrum[CONTRAST_SUB_BAND_END - (i + 1)];
        }
        topAverage /= CONTRAST_BIN_COUNT;
        bottomAverage /= CONTRAST_BIN_COUNT;
        float contrast = constrain((topAverage - bottomAverage) / CONTRAST_MAX_VALUE, 0.0f, 1.0f);
        bright = constrain(bright / 1.6f, 0.0f, 1.0f);

        // Averages
        spectralFlux = rollingAverage(spectralFlux, flux, SMOOTHING_ALPHA);
        spectralCentroid = rollingAverage(spectralCentroid, centroid, SMOOTHING_ALPHA);
        spectralBandwidth = rollingAverage(spectralBandwidth, bandwidth, SMOOTHING_ALPHA);
        spectralContrast = rollingAverage(spectralContrast, contrast, SMOOTHING_ALPHA);
        brightEnergy = rollingAverage(brightEnergy, bright, SMOOTHING_ALPHA);

        // Overall intensity
        float constrained = constrain((amplitudeSum - 0.0175f) / 0.010f, 0.0f, 1.0f);
        constrainedAmplitude = rollingAverage(constrainedAmplitude, constrained, SMOOTHING_ALPHA);

        float centroidContribution = lerp(0.0f, spectralCentroid, constrainedAmplitude);
        float bandwidthContribution = lerp(0.0f, spectralBandwidth, constrainedAmplitude);

        intensity = (0.5f * amplitudeSum) 
                    + (0.2f * spectralFlux) 
                    + (0.1f * centroidContribution) 
                    + (0.1f * bandwidthContribution)
                    + (0.1f * spectralContrast)
                    + (0.1f * brightEnergy);
        intensity = constrain(intensity, 0.0f, 1.0f);

        for (std::function<void()> action : analyzerCallbacks)
            action();
    }
}


float SpectrumAnalyzer::getRawAmplitude()
{
    return rawAmplitude;
}

float SpectrumAnalyzer::getIntensity() 
{
    return intensity;
}

bool SpectrumAnalyzer::getOnset(SoundBand band)
{
    switch(band) 
    {
        case KICK:
            return onsets[0];
            
        case SNARE:
            return onsets[1];

        default:
            return false;
    }
}

float SpectrumAnalyzer::getEnergy(SoundBand band) 
{
    switch(band)
    {
        case KICK:
            return kickEnergy;
        
        case SNARE:
            return snareEnergy;
            
        case BASS:
            return bassEnergy;
            
        case BRIGHTNESS:
            return brightEnergy;
    }
    return 0.0f;
}

float SpectrumAnalyzer::getSpectralParameter(SpectralParameterType parameterType)
{
    switch(parameterType) 
    {
        case FLUX:
            return spectralFlux;
            
        case CENTROID:
            return spectralCentroid;
            
        case BANDWIDTH:
            return spectralBandwidth;
            
        case CONTRAST:
            return spectralContrast;
        
        default:
            return 0;
    }
}

// -------- PRIVATE ----------- ///


float SpectrumAnalyzer::getInstantaneousEnergy(uint16_t start, uint16_t end, uint16_t bandSize)
{
    // float energy = 0.0f;    
    // for (uint8_t i = start; i <= end; i++) 
    // {
    //     energy += pow(fourierAnalyzer.read(i), 2.0f);
    // }

    // return (energy / bandSize);
    return fourierAnalyzer.read(start, end) / bandSize;
}

float SpectrumAnalyzer::getAverageEnergy(RingBuffer<float>& buffer)
{
    float averageEnergy = 0.0f;
    for (uint8_t i = 0; i < FFT_HISTORY_SIZE; i++) 
    {
        averageEnergy += buffer.get(i);
    }
    averageEnergy /= FFT_HISTORY_SIZE;
    return averageEnergy;
}

float SpectrumAnalyzer::getEnergyVariance(RingBuffer<float>& buffer, float averageEnergy)
{
    float variance = 0.0f;
    for (uint8_t i = 0; i < FFT_HISTORY_SIZE; i++) 
    {
        variance += pow(buffer.get(i) - averageEnergy, 2.0f);
    }
    variance /= FFT_HISTORY_SIZE;
    // return variance * 300.0f;
    return variance * 15.0f;    
}

void SpectrumAnalyzer::addCallback(std::function<void()> callback)
{
    analyzerCallbacks.push_back(callback);
}