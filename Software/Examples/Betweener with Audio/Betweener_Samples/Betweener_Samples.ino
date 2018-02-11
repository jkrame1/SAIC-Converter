//This example sketch uses the trigger inputs of the betweener to trigger samples
//
// INCLUDES MOST OF the PJRC TUTORIAL CODE: Advanced Microcontroller-based Audio Workshop
//
// http://www.pjrc.com/store/audio_tutorial_kit.html
// https://hackaday.io/project/8292-microcontroller-audio-workshop-had-supercon-2015
//
// Part 2-3: Playing Samples

// WAV files converted to code by wav2sketch
#include "AudioSampleSnare.h"        // http://www.freesound.org/people/KEVOY/sounds/82583/
#include "AudioSampleTomtom.h"       // http://www.freesound.org/people/zgump/sounds/86334/
#include "AudioSampleHihat.h"        // http://www.freesound.org/people/mhc/sounds/102790/
#include "AudioSampleKick.h"         // http://www.freesound.org/people/DWSD/sounds/171104/
#include "AudioSampleGong.h"         // http://www.freesound.org/people/juskiddink/sounds/86773/



//include the Betweener library
#include <Betweener.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//make a Betweener object.
Betweener b;


// GUItool: begin automatically generated code
AudioPlayMemory          playMem4;       //xy=170,237
AudioPlayMemory          playMem2;       //xy=172,165
AudioPlayMemory          playMem3;       //xy=172,200
AudioPlayMemory          playMem1;       //xy=173,129
AudioMixer4              mixer1;         //xy=397,186
AudioOutputI2S           i2s1;           //xy=550,196
AudioConnection          patchCord1(playMem4, 0, mixer1, 3);
AudioConnection          patchCord2(playMem2, 0, mixer1, 1);
AudioConnection          patchCord3(playMem3, 0, mixer1, 2);
AudioConnection          patchCord4(playMem1, 0, mixer1, 0);
AudioConnection          patchCord5(mixer1, 0, i2s1, 0);
AudioConnection          patchCord6(mixer1, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=539,273
// GUItool: end automatically generated code



void setup() {

  pinMode(8, OUTPUT); //set Betweener LED Pin to output.

  AudioMemory(8);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.5);
  mixer1.gain(0, 0.4);
  mixer1.gain(1, 0.4);
  mixer1.gain(2, 0.4);
  mixer1.gain(3, 0.4);
  //the Betweener begin function is necessary before it will do anything
  b.begin();
}

void loop() {
  //this function reads just the triggers. We could also do readAllInputs().
  b.readTriggers();

  if (b.trig1.fallingEdge()) {
    playMem1.play(AudioSampleKick);
  }
  if (b.trig2.fallingEdge()) {
    playMem2.play(AudioSampleSnare);
  }
  if (b.trig3.fallingEdge()) {
    playMem3.play(AudioSampleHihat);
  }
  if (b.trig4.fallingEdge()) {
    playMem4.play(AudioSampleTomtom);
  }
  if ((playMem1.isPlaying()) || (playMem2.isPlaying()) || (playMem3.isPlaying()) || (playMem4.isPlaying())) {
    digitalWrite(8, HIGH);
  }
  else {
    digitalWrite(8, LOW);
  }
}

