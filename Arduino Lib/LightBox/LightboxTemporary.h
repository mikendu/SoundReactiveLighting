#ifndef LIGHT_BOX_TEMP_H
#define LIGHT_BOX_TEMP_H


#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>


#include "MillisecondTimer.h"
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/
#include "AudioSampleCashregister.h" // http://www.freesound.org/people/kiddpark/sounds/201159/



AudioInputI2S            audioIn;
AudioOutputI2S           audioOut;
AudioControlSGTL5000     audioControl;
AudioMixer4              mixer;

AudioAnalyzeFFT256       analyzer;
AudioAnalyzePeak         peakLeft;
AudioAnalyzePeak         peakRight;
AudioAnalyzePeak         peakMiddle;


AudioPlayMemory          playMem3;       //xy=185,625.0000095367432
AudioPlayMemory          playMem1;       //xy=190,514.0000114440918
AudioPlayMemory          playMem4;       //xy=190,696
AudioPlayMemory          playMem2;       //xy=193.00000381469727,561.0000133514404

// Connections
//AudioConnection          patchCord1(audioIn, 0, mixer, 0);
//AudioConnection          patchCord2(audioIn, 1, mixer, 1);
AudioConnection          patchCord3(mixer, 0, peakLeft, 0);
AudioConnection          patchCord4(mixer, 0, peakRight, 0);
AudioConnection          patchCord5(mixer, 0, audioOut, 0);
AudioConnection          patchCord6(mixer, 0, audioOut, 1);
AudioConnection          patchCord7(mixer, analyzer);

AudioConnection          patchCord8(playMem1, 0, mixer, 0);
AudioConnection          patchCord9(playMem2, 0, mixer, 1);
AudioConnection          patchCord10(playMem3, 0, mixer, 2);
AudioConnection          patchCord11(playMem4, 0, mixer, 3);

AudioConnection          patchCord18(mixer, peakMiddle);


uint8_t beatCounter = 0;
float timeCounter = 0;
bool beatsEnabled = true;
uint8_t totalBeats = 0;


uint8_t channel1[16] = { 1, 0, 0, 1,    0, 1, 0, 1,     1, 1, 0, 1,    1, 0, 0, 1 };
uint8_t channel2[16] = { 0, 0, 1, 0,    0, 0, 1, 0,     0, 0, 1, 0,    0, 0, 1, 0 };
uint8_t channel3[16] = { 1, 1, 1, 1,    1, 1, 1, 1,     1, 1, 1, 1,    1, 1, 1, 1 };
uint8_t channel4[16] = { 1, 0, 0, 0,    0, 0, 0, 0,     1, 0, 0, 0,    0, 0, 0, 0 };/*
uint8_t channel1[16] = { 1, 0, 0, 0,    0, 0, 0, 0,     1, 0, 0, 0,    0, 0, 0, 0 };
uint8_t channel2[16] = { 0, 0, 0, 0,    1, 0, 0, 0,     0, 0, 0, 0,    1, 0, 0, 0 };
uint8_t channel3[16] = { 0, 0, 0, 0,    0, 0, 0, 0,     0, 0, 0, 0,    0, 0, 0, 0 };
uint8_t channel4[16] = { 0, 0, 0, 0,    0, 0, 0, 0,     0, 0, 0, 0,    0, 0, 0, 0 };*/

void setupTemp()
{
    mixer.gain(0, 0.8);
    mixer.gain(1, 0.8);
    mixer.gain(2, 0.08);
    mixer.gain(3, 0.2);
  

    //AudioSampleSnare
    //AudioSampleKick
    //AudioSampleTomtom
    //AudioSampleHihat
    //AudioSampleGong
    //AudioSampleCashregister
    audioControl.volume(0.8);
}

void onBeat(uint8_t beatNumber)
{
    totalBeats += 1;
    if(totalBeats >= 32)
    {
        beatsEnabled = !beatsEnabled;
        totalBeats = 0;
    }


    if(beatsEnabled)
    {
        if(channel1[beatNumber] > 0)
            playMem1.play(AudioSampleKick);

        if(channel2[beatNumber] > 0)
            playMem2.play(AudioSampleSnare);

        if(channel3[beatNumber] > 0)
            playMem3.play(AudioSampleHihat);

        if(channel4[beatNumber] > 0)
            playMem4.play(AudioSampleCashregister);
    }
}


void updateTemp()
{
    timeCounter += MillisecondTimer::elapsed();
    if(timeCounter >= 250)
    {
        timeCounter -= 250.0f;

        onBeat(beatCounter);
        beatCounter = (beatCounter + 1) % 16;
    }

}

#endif