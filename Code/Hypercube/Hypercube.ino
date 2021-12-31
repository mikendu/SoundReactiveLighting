

#define INDICATOR_LIGHT_PIN 2
#define TOUCH_SWITCH_PIN 22
#define LED_COUNT 201 // Should be 301 with external strip, 241 with internal strip from cube. Set to 201 since some lights are broken

#include <LightBox.h>

///////////////////////////////////
// Touch Inputs & LED Strip
///////////////////////////////////

NeopixelStrip           ledStrip;
TouchPotentiometer      potentiometer;
TouchButton             touchButton(TOUCH_SWITCH_PIN);
BrightnessController    brightnessController;
bool                    calibrationMode = false;


///////////////////////////////////
// Audio Input & Routing
///////////////////////////////////

AudioInputI2S           audioIn;
AudioControlSGTL5000    audioControl;
AudioMixer4             mixer;

AudioAnalyzeFFT1024     fourierAnalyzer;
AudioAnalyzePeak        peakMiddle;

//AudioConnection       patchCord1(audioIn, 0, mixer, 0);
AudioConnection         patchCord2(audioIn, 1, mixer, 1);
AudioConnection         patchCord3(mixer, peakMiddle);
AudioConnection         patchCord4(mixer, fourierAnalyzer);


///////////////////////////////////
// Audio Processing & Analysis
///////////////////////////////////

GainCalibrator          gainCalibrator(peakMiddle, ledStrip, 20);
SpectrumAnalyzer        spectrumAnalyzer(fourierAnalyzer, peakMiddle);


///////////////////////////////////
// Visuals
///////////////////////////////////

VisualsManager          visualsManager(spectrumAnalyzer, ledStrip);








void setup() 
{
    Serial.begin(115200);
    brightnessController.initialize(&potentiometer, &touchButton);
    ledStrip.initialize(LED_COUNT, brightnessController.getBrightness());  

    // Audio
    AudioMemory(16);
    audioControl.enable();
    audioControl.inputSelect(AUDIO_INPUT_LINEIN);
    audioControl.micGain(0);
    fourierAnalyzer.windowFunction(AudioWindowHanning1024);

    // Indicator
    pinMode(INDICATOR_LIGHT_PIN, OUTPUT);
    digitalWrite(INDICATOR_LIGHT_PIN, HIGH);
}

void loop() 
{    
    touchButton.update();
    if (touchButton.doubleTapped())
    {
        calibrationMode = !calibrationMode;
        ledStrip.clearNow();
    }
    
    
    if (calibrationMode) 
    {
        ledStrip.setBrightness(brightnessController.update());
        gainCalibrator.update();
    }
    else 
    {    
        // clear leds
        ledStrip.clear();

        // process sound & fill color array with patterns
        spectrumAnalyzer.update();        
        visualsManager.update();

        // show colors on actual leds
        ledStrip.setBrightness(brightnessController.update());
        ledStrip.show();    

    }
}
