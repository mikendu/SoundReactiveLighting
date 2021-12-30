#ifndef BEAT_ANALYZER_H
#define BEAT_ANALYZER_H

#define FFT_BIN_COUNT 512
#define FFT_BIN_WIDTH 43
#define FFT_HISTORY_SIZE 43
#define THRESHOLD 1.55f
#define NUM_BANDS 2

#define CENTROID_MAX_FREQUENCY 4000.0f
#define CENTROID_MIN_FREQUENCY 70.0f
#define CONTRAST_SUB_BAND_START 1
#define CONTRAST_SUB_BAND_END 127
#define CONTRAST_BIN_COUNT 25
#define CONTRAST_MAX_VALUE 0.0375f

#define SMOOTHING_ALPHA 0.15f
#define ONSET_TRIGGER_ON_THRESHOLD 0
#define ONSET_TRIGGER_OFF_THRESHOLD 7

#include "Common.h"
#include "Debugger.h"
#include "MillisecondTimer.h"

struct FrequencyBand
{
    float minFrequency;
    float maxFrequency;
    float minEnergy;
    uint8_t binStart;
    uint8_t binEnd;
    uint8_t binWidth;
};

enum SoundBand
{
    KICK,
    BASS,
    SNARE,
    BRIGHTNESS
};

enum SpectralParameterType
{
    FLUX,
    CENTROID,
    BANDWIDTH,
    CONTRAST
};

class SpectrumAnalyzer
{

public:

    SpectrumAnalyzer(       AudioAnalyzeFFT1024& fft, 
                            AudioAnalyzePeak& peak);


    void                    update();
    float                   getIntensity();
    float                   getRawAmplitude();
    bool                    getOnset(SoundBand band);
    float                   getSpectralParameter(SpectralParameterType parameterType);
    float                   getEnergy(SoundBand band);
    void                    addCallback(std::function<void()> callback);


private:

    AudioAnalyzeFFT1024&    fourierAnalyzer;
    AudioAnalyzePeak&       peakAnalyzer;
    float                   intensity;

    RingBuffer<float>       spectrumHistory[NUM_BANDS];
    FrequencyBand*          frequencyBands;
    bool                    onsets[NUM_BANDS];
    bool                    onsetHistory[NUM_BANDS];
    uint8_t                 onsetCounters[NUM_BANDS];
    
    float                   spectrum[FFT_BIN_COUNT];
    float                   previousSpectrum[FFT_BIN_COUNT];
    float                   energySpectrum[FFT_BIN_COUNT];
    float                   spectralFlux;
    float                   spectralCentroid;
    float                   spectralBandwidth;
    float                   spectralContrast;
    float                   amplitudeSum;
    float                   constrainedAmplitude;
    float                   rawAmplitude;
    
    float                   brightEnergy;
    float                   kickEnergy;
    float                   bassEnergy;
    float                   snareEnergy;

    // Derived
    float                   kickSquared;
    float                   intensityDouble;
    float                   speedIncrease;

    float                   getInstantaneousEnergy(uint16_t start, uint16_t end, uint16_t bandSize);
    float                   getAverageEnergy(RingBuffer<float>& buffer);
    float                   getEnergyVariance(RingBuffer<float>& buffer, float averageEnergy);

    std::vector<std::function<void()>> analyzerCallbacks;
};

#endif

