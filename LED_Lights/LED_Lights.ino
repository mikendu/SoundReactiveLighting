#define DEFAULT_UPDATE_SPEED 0.0001f
//#define DEBUG_MODE

#include <TeensyThreads.h>
#include <LightBox.h>
#include <Bounce2.h>

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


///////////////////////////////////
// Audio Input & Analysis
///////////////////////////////////

AudioInputI2S            audioIn;
AudioOutputI2S           audioOut;
AudioControlSGTL5000     audioControl;
AudioMixer4              mixer;

AudioAnalyzeFFT1024      analyzer;
AudioAnalyzePeak         peakLeft;
AudioAnalyzePeak         peakRight;
AudioAnalyzePeak         peakMiddle;

// Connections
AudioConnection          patchCord1(audioIn, 0, mixer, 0);
AudioConnection          patchCord2(audioIn, 1, mixer, 1);
AudioConnection          patchCord3(audioIn, 0, peakLeft, 0);
AudioConnection          patchCord4(audioIn, 1, peakRight, 0);
AudioConnection          patchCord5(audioIn, 0, audioOut, 0);
AudioConnection          patchCord6(audioIn, 1, audioOut, 1);
AudioConnection          patchCord7(mixer, analyzer);
AudioConnection          patchCord8(mixer, peakMiddle);



//const uint8_t LogarithmicBinSizes[16] = { 1, 1, 2, 2, 2, 3, 4, 5, 6, 7, 9, 11, 13, 16, 20, 25 };
//const uint8_t LogarithmicBinSizes[16] = { 1, 1, 1, 2, 2, 2, 3, 3, 3, 4, 5, 5, 6, 7, 9, 10 };
//const uint8_t LogarithmicBinSizes[16] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1 };


///////////////////////////////////
// Display & Control
///////////////////////////////////
Bounce modeButton = Bounce(17, 15);
Potentiometer sliderOne = Potentiometer(A1);
Potentiometer sliderTwo = Potentiometer(A2);
Potentiometer* activeSlider = nullptr;

CustomDisplay screen = CustomDisplay();
Bundle peakData = Bundle();

float eqData[16] = { 0 };
float fullSpectrum[512] = { 0 };

#ifdef DEBUG_MODE
char textBuffer[32] = { 0 };
uint16_t textLength = 0;
#endif

bool adjustmentMode = false;
float adjustmentTimer = 0.0f;
bool coloredSlider = false;


///////////////////////////////////
// LED Lighting
///////////////////////////////////

AnalogLED ledStrip = AnalogLED(2, 1, 0);
LightMode lightMode = LightMode::Dynamic;

float updateSpeed = DEFAULT_UPDATE_SPEED;
float updateCounter = 0.0f;

ColorCycler colorCycler = ColorCycler();
BeatAnalyzer beatAnalyzer = BeatAnalyzer();
KickTimer kickEffectTimer = KickTimer(10, 10, 180);
float highAverage = 0.0f;
float bassAverage = 0.0f;
float powerAverage = 0.0f;

HoldTimer releaseTimer = HoldTimer(0.0f, 200, 1000);
DecayTimer saturationTimer = DecayTimer(400);

SinusoidTimer hueCycle = SinusoidTimer(14456);
SinusoidTimer saturationCycle = SinusoidTimer(17856);
SinusoidTimer brightnessCycle = SinusoidTimer(30356);


void setup() 
{
    /// --- BASICS --- ///
    Serial.begin(115200);
    analogWrite(22, 255);   // "On" LED Indicator


    /// --- AUDIO --- ///
    mixer.gain(0, 0.5);
    mixer.gain(1, 0.5);
    analyzer.windowFunction(AudioWindowHanning1024);
    
    AudioMemory(16);
    audioControl.enable();
    

    audioControl.inputSelect(AUDIO_INPUT_LINEIN);
    audioControl.micGain(36);

    
    /// --- DISPLAY --- ///
    screen.initialize();
    setMode(lightMode);


    /// --- LIGHTING --- ///
    ledStrip.setup();

    
   
    /// --- THREADS --- ///
    threads.addThread(displayThread);
    threads.setTimeSlice(0, 1);
    threads.setTimeSlice(1, 1);
}


void loop()
{
    /// --- TIMING --- ///
    
    MillisecondTimer::update();
    float delta = MillisecondTimer::elapsed();
    updateCounter = LightUtils::wrap(updateCounter + (updateSpeed * delta), 1.0f);
    
    /// --- CONTROL LOGIC --- ///
    updateAudio();


    /// --- UPDATE LIGHTS --- ///
    updateLights();
}


void displayThread()
{
    
    while(true)
    {
        if(adjustmentMode && activeSlider != nullptr)
            screen.drawSlider(activeSlider->value(), coloredSlider);
        else
            screen.drawAudio(eqData, 16, peakData.value1, peakData.value2, peakData.value3);

#ifdef DEBUG_MODE
        screen.debugText(textBuffer);
#endif

        // Target a ~60 FPS Update Rate
        threads.delay(18);

        // Updates
        float delta = MillisecondTimer::elapsed();
        updateInputs(delta);
    }
}


void setMode(LightMode mode)
{
    lightMode = mode;
    switch(mode)
    {
        case LightMode::Dynamic:
            updateSpeed = DEFAULT_UPDATE_SPEED;
            screen.drawTitle("Mode: Dynamic");
            break;

        case LightMode::Fade:
            updateSpeed = DEFAULT_UPDATE_SPEED;
            screen.drawTitle("Mode: Fade");
            break;
        
        case LightMode::Hue:
            updateSpeed = DEFAULT_UPDATE_SPEED;
            screen.drawTitle("Mode: Hue");
            break;

        case LightMode::Psycho:
            updateSpeed = DEFAULT_UPDATE_SPEED;
            screen.drawTitle("Mode: Psycho");
            break;

        default:
            break;
    }
}

void updateAudio()
{
    peakData.value1 = peakLeft.available() ? peakLeft.read() : peakData.value1;
    peakData.value2 = peakRight.available() ? peakRight.read() : peakData.value2;
    peakData.value3 = peakMiddle.available() ? peakMiddle.read() : peakData.value3;
    

    if (analyzer.available())
    {
        for(uint16_t i = 0; i < 512; i++)
        {
            float power = analyzer.read(i);
            fullSpectrum[i] = power;
            if (i < 16)
                eqData[i] = power;
        }
    }
}
float rollingPower = 0.0f;

void updateLights()
{
    float masterBrightness = sliderOne.scaledValue();
    float parameter = sliderTwo.value();
    switch(lightMode)
    {
        case LightMode::Dynamic:
        {
            releaseTimer.update(peakData.value3);
            float interpolation =  releaseTimer.value();
            beatAnalyzer.update(fullSpectrum, peakData);

            rollingPower = LightUtils::rollingAverage(rollingPower, beatAnalyzer.weightedPower(), 0.1f);
            float power = rollingPower;
            float bassPower = beatAnalyzer.bassPower();
            float bassCenter = beatAnalyzer.bassCenter();
            float highPower = beatAnalyzer.highPower();
            float kick = beatAnalyzer.kickPower();  
            kickEffectTimer.update(kick);     

 

            // -- IDLE STATE -- //

            powerAverage = LightUtils::rollingAverage(powerAverage, power, 0.00009f);
            float multiplier = 0.43f + (1.3f * powerAverage);
            //powerAverage = min(powerAverage, power);
            
            
            hueCycle.update(multiplier);
            saturationCycle.update(multiplier);
            brightnessCycle.update(multiplier);
            
            uint8_t hue = (uint8_t) round(255 * hueCycle.value());
            uint8_t activeSat = (uint8_t) round(255 * saturationCycle.scaledValue(0.6f, 0.8f));
            uint8_t idleSat = (uint8_t) round(255 * saturationCycle.scaledValue(0.25f, 0.75f));
            uint8_t val = 255;

            CHSV idleColor = CHSV(hue, idleSat, val);
            float idleBrightness = brightnessCycle.scaledValue(0.7f * parameter, 1.0f * parameter);
    
            CHSV currentColor = idleColor;
            float currentBrightness = idleBrightness;
            
            if (interpolation < 1.0f)
            {
                currentBrightness = 1.0f;
                currentColor = CHSV(hue, activeSat, val);
                
                saturationTimer.update(LightUtils::clamp(power + powerAverage, 0.0f, 1.0f));
                uint8_t boostedSaturation = (uint8_t)round(LightUtils::lerp(currentColor.sat, 255, saturationTimer.value()));
                currentColor = CHSV(currentColor.hue, boostedSaturation, currentColor.val);
                
                // -- GENERAL -- //

                //float powerScaling = LightUtils::lerp(1.25f, 1.0f, pow(powerAverage, 0.575f));
                float totalScaling = LightUtils::clamp(0.5f + (powerAverage * 0.8f), 0.0f, 1.0f);
                float dim = 1.0f - power;
                currentBrightness = totalScaling * dim;
                
                
                // -- BASS -- //
                
                bassAverage = LightUtils::rollingAverage(bassAverage, bassPower, 0.00009f);
                bassAverage = min(bassAverage, bassPower);
                
                float bassFactor = (bassPower * 0.325f) + (0.25f * bassAverage);   
                float bassBrighten = (bassPower * 0.07f) + (0.65f * bassAverage);         
                uint8_t bassBlend = (uint8_t)round(255 * bassFactor);
                
                float bassFrequency = LightUtils::lerp(37.0f, 51.0f, bassCenter);
                CHSV bassEffect = ledStrip.flicker(currentColor, bassFrequency);
                
                currentColor = blend(currentColor, bassEffect, bassBlend);
                currentBrightness = LightUtils::lerp(currentBrightness, 1.0f, bassBrighten);
    
                
                // -- HIGH -- //
    
                highAverage = LightUtils::rollingAverage(highAverage, highPower, 0.00009f);
                highAverage = min(highAverage, highPower);
                
                float highFactor = (highPower * 0.12f) + (highAverage * 0.36f);
                uint8_t highBlend = (uint8_t)round(255 * highFactor);    
                        
                CHSV highEffect = colorCycler.randomColor();            
                currentColor = blend(currentColor, highEffect, highBlend);
                float overBright = LightUtils::clamp(currentBrightness * 2.0f, 0.0f, 1.0f);
                currentBrightness = LightUtils::lerp(currentBrightness, overBright, highAverage * 0.72f);
                
    
                // -- KICK -- //
                if(kickEffectTimer.enabled())
                {                
                    uint8_t sat = (uint8_t)round(currentColor.sat * kickEffectTimer.saturation());  
                    currentColor = CHSV(currentColor.hue, sat, currentColor.val);
                    currentBrightness = kickEffectTimer.value();
                }

                uint8_t finalBlend = (uint8_t)round(255 * interpolation);
                currentColor = blend(currentColor, idleColor, finalBlend);
                currentBrightness = LightUtils::lerp(currentBrightness, idleBrightness, interpolation);
            }

            ledStrip.show(currentColor, masterBrightness * currentBrightness);



#ifdef DEBUG_MODE
            textLength = sprintf(textBuffer, "%0.02f", powerFloor);         
#endif

            break;
        }

        case LightMode::Fade:
        {
            uint8_t hue = (uint8_t) round(255 * updateCounter);
            uint8_t sat = (uint8_t) round(255 * parameter);
            ledStrip.show(CHSV(hue, sat, 255), masterBrightness);
            
            break;
        }
        
        case LightMode::Hue:
        {
            uint8_t hue = (uint8_t) round(255 * parameter);
            ledStrip.show(CHSV(hue, 255, 255), masterBrightness);
            
            break;
        }

        case LightMode::Psycho:
        {
            ledStrip.show(colorCycler.randomColor(), masterBrightness);
            
            break;
        }

        default:
            break;
    }
}

void updateInputs(float delta)
{
    modeButton.update();
    sliderOne.update();
    sliderTwo.update();
    if(modeButton.rose())
    {
        LightMode newMode = (LightMode)((lightMode + 1) % 4);
        setMode(newMode);
    }

    if(adjustmentMode)
    {
        adjustmentTimer += delta;
        if(adjustmentTimer > 0.05f)
        {
            adjustmentTimer = 0.0f;
            adjustmentMode = false;
            screen.clear();
            setMode(lightMode);
            activeSlider = nullptr;
        }
    }

    if(sliderOne.available())
    {
        if(!adjustmentMode || activeSlider != &sliderOne)
        {
            screen.clear();
            screen.sliderText("Brightness");
        }
            
        adjustmentTimer = 0.0f;
        adjustmentMode = true;
        activeSlider = &sliderOne;
        coloredSlider = true;
    }

    if(sliderTwo.available())
    {
        if(!adjustmentMode || activeSlider != &sliderTwo)
        {
            screen.clear();
            screen.sliderText("Parameter");
        }
            
        adjustmentTimer = 0.0f;
        adjustmentMode = true;
        activeSlider = &sliderTwo;
        coloredSlider = false;
    }
}
