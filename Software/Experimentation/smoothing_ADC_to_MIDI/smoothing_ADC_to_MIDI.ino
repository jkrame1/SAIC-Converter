#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;

// create variables to store current and previous values of smoothed knob
int PrevOut1 = -1;
int CurrentOut1 = -1;

//create variables for MIDI CC channel
int CC5 = 24;

//////////////////////////////////////////
//Filtering Algorithm from https://www.norwegiancreations.com/2015/10/tutorial-potentiometers-with-arduino-and-filtering/

int Knob1 = 0;    //initialization of sensor variable, equivalent to EMA Y
float EMA_a = .3;      //initialization of EMA alpha - lower is slower and smoother
int EMA_S = 0;          //initialization of EMA S


void setup() {
  //the Betweener begin function is necessary before it will
  //do anything
  b.begin();
  Serial.begin(115200);
  EMA_S = b.readKnobMIDI(1);  //set EMA S for t=1
}


void loop() {
  Knob1 = b.readKnobMIDI(1);                         //read the sensor value using ADC
  EMA_S = (EMA_a * Knob1) + ((1 - EMA_a) * EMA_S);   //run the EMA
//  int  out1 = map(EMA_S,0,1023,0,127);
  int  CurrentOut1 = EMA_S >> 3;
 // Serial.println(CurrentOut1);                                    //print digital value to serial

    if (CurrentOut1 != PrevOut1) {
      usbMIDI.sendControlChange(CC5, CurrentOut1, channel);
      PrevOut1 = CurrentOut1;
      Serial.println(CurrentOut1);
    }


  delay(50);

  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}

