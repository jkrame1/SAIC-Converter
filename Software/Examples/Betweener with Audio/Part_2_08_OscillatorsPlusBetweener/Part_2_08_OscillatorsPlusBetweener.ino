// Advanced Microcontroller-based Audio Workshop
//
// http://www.pjrc.com/store/audio_tutorial_kit.html
// https://hackaday.io/project/8292-microcontroller-audio-workshop-had-supercon-2015
//
// Part 2-8: Oscillators
//
//This example has been modified to use the Betweener hardware's knobs and trigger inputs
//as the iputs instead of the Audio Shield Tutorial kit

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
AudioSynthWaveformSine   sine1;          //xy=147,230
AudioSynthNoisePink      pink1;          //xy=149,270
AudioSynthWaveform       waveform1;      //xy=152,148
AudioSynthWaveformSineModulated sine_fm1;       //xy=152,192
AudioMixer4              mixer1;         //xy=305,203
AudioEffectEnvelope      envelope1;      //xy=434,251
AudioMixer4              mixer2;         //xy=568,221
AudioOutputI2S           i2s1;           //xy=693,224
AudioConnection          patchCord1(sine1, 0, mixer1, 2);
AudioConnection          patchCord2(pink1, 0, mixer1, 3);
AudioConnection          patchCord3(waveform1, sine_fm1);
AudioConnection          patchCord4(waveform1, 0, mixer1, 0);
AudioConnection          patchCord5(sine_fm1, 0, mixer1, 1);
AudioConnection          patchCord6(mixer1, envelope1);
AudioConnection          patchCord7(mixer1, 0, mixer2, 0);
AudioConnection          patchCord8(envelope1, 0, mixer2, 1);
AudioConnection          patchCord9(mixer2, 0, i2s1, 0);
AudioConnection          patchCord10(mixer2, 0, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=394,358
// GUItool: end automatically generated code





void setup() {
  Serial.begin(9600);
 
   pinMode(8, OUTPUT); //set Betweener LED Pin to output.
  //the Betweener begin function is necessary before it will do anything
  b.begin();
  
  AudioMemory(20);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.32);
  
  mixer1.gain(0, 0.75);
  mixer1.gain(1, 0.0);
  mixer1.gain(2, 0.0);
  mixer1.gain(3, 0.0);
  mixer2.gain(0, 0.15);
  mixer2.gain(1, 0.0);
  mixer2.gain(2, 0.0);
  mixer2.gain(3, 0.0);
  waveform1.begin(WAVEFORM_SAWTOOTH);
  waveform1.amplitude(0.75);
  waveform1.frequency(50);
  waveform1.pulseWidth(0.15);
  sine_fm1.frequency(440);
  sine_fm1.amplitude(0.75);
  sine1.frequency(200);
  sine1.amplitude(0.75);
  pink1.amplitude(0.75);
  envelope1.attack(10);
  envelope1.hold(10);
  envelope1.decay(25);
  envelope1.sustain(0.4);
  envelope1.release(70);
}

int waveform_type = WAVEFORM_SAWTOOTH;
int mixer1_setting = 0;
int mixer2_setting = 0;
elapsedMillis timeout = 0;
bool mixer2_envelope = false;

void loop() {
  //this function reads just the triggers. We could also do readAllInputs().
  b.readTriggers();


  // Left changes the type of control waveform
  if (b.trig1.fallingEdge()) {
    Serial.print("Control waveform: ");
    if (waveform_type == WAVEFORM_SAWTOOTH) {
      waveform_type = WAVEFORM_SINE;
      Serial.println("Sine");
    } else if (waveform_type == WAVEFORM_SINE) {
      waveform_type = WAVEFORM_SQUARE;
      Serial.println("Square");
    } else if (waveform_type == WAVEFORM_SQUARE) {
      waveform_type = WAVEFORM_TRIANGLE;
      Serial.println("Triangle");
    } else if (waveform_type == WAVEFORM_TRIANGLE) {
      waveform_type = WAVEFORM_PULSE;
      Serial.println("Pulse");
    } else if (waveform_type == WAVEFORM_PULSE) {
      waveform_type = WAVEFORM_SAWTOOTH;
      Serial.println("Sawtooth");
    }
    waveform1.begin(waveform_type);
  }

  // middle button switch which source we hear from mixer1
  if (b.trig2.fallingEdge()) {
    if (mixer1_setting == 0) {
      mixer1.gain(0, 0.75);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: Control oscillator");
      mixer1_setting = 1;
    } else if (mixer1_setting == 1) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.75);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: Frequency Modulated Oscillator");
      mixer1_setting = 2;
    } else if (mixer1_setting == 2) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.75);
      mixer1.gain(3, 0.0);
      Serial.println("Mixer1: Regular Sine Wave Oscillator");
      mixer1_setting = 3;
    } else if (mixer1_setting == 3) {
      mixer1.gain(0, 0.0);
      mixer1.gain(1, 0.0);
      mixer1.gain(2, 0.0);
      mixer1.gain(3, 0.75);
      Serial.println("Mixer1: Pink Noise");
      mixer1_setting = 0;
    }
  }

  // Right button activates the envelope
  if (b.trig3.fallingEdge()) {
    mixer2.gain(0, 0.0);
    mixer2.gain(1, 1.0);
    mixer2_envelope = true;
    timeout = 0;
    envelope1.noteOn();
  }
  if (b.trig3.risingEdge()) {
    envelope1.noteOff();
    timeout = 0;
  }

  // after 4 seconds of inactivity, go back to
  // steady listening intead of the envelope
  if (mixer2_envelope == true && timeout > 4000) {
    mixer2.gain(0, 0.15);
    mixer2.gain(1, 0.0);
    mixer2_envelope = false;
  }

  // use the knobs to adjust parameters
//KNOB1 is a variable that is declared in the Betweener library
  float knob1 = (float)analogRead(KNOB1) / 1023.0;
  float knob2 = (float)analogRead(KNOB2) / 1023.0;
  float knob3 = (float)analogRead(KNOB3) / 1023.0;
  waveform1.frequency(360 * knob2 + 0.25);
  sine_fm1.frequency(knob3 * 1500 + 50);
  sine1.frequency(knob3 * 1500 + 50);
}


