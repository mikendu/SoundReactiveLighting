#define DEFAULT_UPDATE_SPEED 0.0001f

#include <TeensyThreads.h>
#include <LightBox.h>
//#include <LightboxTemporary.h>
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

bool adjustmentMode = false;
float adjustmentTimer = 0.0f;
bool coloredSlider = false;


///////////////////////////////////
// LED Lighting
///////////////////////////////////

CHSV ledColor = CHSV();
AnalogLED ledStrip = AnalogLED(2, 1, 0);
LightMode lightMode = LightMode::Dynamic;

float updateSpeed = DEFAULT_UPDATE_SPEED;
float updateCounter = 0.0f;

DecayTimer dimTimer = DecayTimer(100);
DecayTimer speedTimer = DecayTimer(100);
DecayTimer kickDim = DecayTimer(100, 1.0f);
DecayTimer kickDesaturate = DecayTimer(300, 0.7f);

HoldTimer releaseTimer = HoldTimer(0.0f, 200, 1000);
SinusoidTimer idleHue = SinusoidTimer(14456);
SinusoidTimer idleSat = SinusoidTimer(17856);
SinusoidTimer idleBrightness = SinusoidTimer(30356);


void setup() 
{
    /// --- BASICS --- ///
    Serial.begin(115200);
    analogWrite(22, 255);   // "On" LED Indicator


    /// --- AUDIO --- ///
    mixer.gain(0, 0.5);
    mixer.gain(1, 0.5);
    
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
        /*uint8_t startBin = 0;
        for(uint8_t i = 0; i < 16; i++)
        {
            uint8_t binSize = LogarithmicBinSizes[i];
            uint8_t endBin = startBin + binSize;
            float power = analyzer.read(startBin, endBin);
            eqData[i] = power;
            startBin += binSize;
        }*/
        for(uint16_t i = 0; i < 512; i++)
        {
            float power = analyzer.read(i);
            fullSpectrum[i] = power;
            if (i < 16)
                eqData[i] = power;
        }
    }
}

void updateLights()
{
    float masterBrightness = sliderOne.scaledValue();
    float parameter = sliderTwo.value();
    switch(lightMode)
    {
        case LightMode::Dynamic:
        {
            releaseTimer.update(peakData.value3);
            float interpolation = releaseTimer.value();
            float multiplier = 0.25f + (2.25f * parameter);
            
            uint8_t hue, sat, val;
            float colorBrightness;

            /// --- IDLE STATE --- ///
            idleHue.update(multiplier);
            idleSat.update(multiplier);
            idleBrightness.update(multiplier);
            
            hue = (uint8_t) (255 * idleHue.value());
            sat = (uint8_t) (255 * idleSat.value());
            val = 255;
            colorBrightness = idleBrightness.scaledValue(0.3f, 0.4f);
            
            if (interpolation < 1.0f)
            {
                
                float speedScale = 0.25f + (parameter * 1.5f);   
                speedTimer.update(peakData.value3);         
                updateSpeed = DEFAULT_UPDATE_SPEED + (1.5f * DEFAULT_UPDATE_SPEED * speedTimer.value());
                updateSpeed *= speedScale;

                float totalPower = LightUtils::sum(fullSpectrum, 512) / 16;
                
                dimTimer.update(peakData.value3 * totalPower);
                float activeBrightness = 1.0f - dimTimer.value();
                /*
                float kickPower = eqData[0] + eqData[1];
                kickPower *= kickPower;
                
                kickDim.update(kickPower);
                kickDesaturate.update(kickPower);

                float saturationOffset = (100 * idleSat.value());
                uint8_t h = (uint8_t) round(updateCounter * 255.0f);
                float s = LightUtils::clamp(255 * (1.0f - kickDesaturate.value()) - saturationOffset, 0, 255);
                float v = LightUtils::clamp(255 * (1.0f - kickDim.value()), 0, 255);*/
                uint8_t h = (uint8_t) round(updateCounter * 255.0f);
                float s = 255;
                float v = 255;

                if(interpolation > 0.0f)
                {
                    hue = (uint8_t)LightUtils::lerp(h, hue, interpolation);
                    sat = (uint8_t)LightUtils::lerp(s, sat, interpolation);
                    val = (uint8_t)LightUtils::lerp(v, val, interpolation);
                    colorBrightness = LightUtils::lerp(activeBrightness, colorBrightness, interpolation);
                }
                else
                {
                    hue = (uint8_t)h;
                    sat = (uint8_t)s;
                    val = (uint8_t)v;
                    colorBrightness = activeBrightness;
                }
            }
            
            ledColor = CHSV(hue, sat, val);
            ledStrip.show(ledColor, masterBrightness * colorBrightness);
            
            break;
        }

        case LightMode::Fade:
        {
            uint8_t hue = (uint8_t) round(255 * updateCounter);
            uint8_t sat = (uint8_t) round(255 * parameter);
            
            ledColor = CHSV(hue, sat, 255);
            ledStrip.show(ledColor, masterBrightness);
            
            break;
        }
        
        case LightMode::Hue:
        {
            uint8_t hue = (uint8_t) round(255 * parameter);
            ledColor = CHSV(hue, 255, 255);
            ledStrip.show(ledColor, masterBrightness);
            
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
        LightMode newMode = (LightMode)((lightMode + 1) % 3);
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
