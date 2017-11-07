//////////////////////////////////////////////////
//This code is designed to take in some MIDI 
//notes and velocity and aftertouch 
//and turn it into CV outputs
//
//It is currently incomplete - "gate" should only
//go low when the last note is released
//
//////////////////////////////////////////////////

#include <Betweener.h>

Betweener b;
// the MIDI channel number to send messages
const int channel = 1;


void setup() {
  Serial.begin(115200);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleVelocityChange(OnVelocityChange);
  usbMIDI.setHandleAfterTouch(OnAfterTouch);
}

void loop() {
  b.readAllInputs();  // ask the betweener to read all the physical inputs and MIDI
}

void OnNoteOn(byte channel, byte note, byte velocity) {
  int noteCV = map(note, 0, 127, 0, 4095); //turn noteON messages into a CV range
  b.writeCVOut(noteCV, 1); //When a note-on is received, write a CV value scaled to the note value
  b.writeCVOut(4095, 2); //When a note-on is received, write a gate HIGH on output 2
  int velocityCV = map(velocity, 0, 127, 20, 4095); //scale MIDI velocity values to 12bit range for CV output
  b.writeCVOut(velocityCV, 3); //Write velocity CV on channel 3 output
}

void OnNoteOff(byte channel, byte note, byte velocity) {
  b.writeCVOut(0, 2); //When a note-off is received, write a gate LOW on output 2
}


void OnAfterTouch(byte channel, byte pressure) {
  int pressureCV = map(pressure, 0, 127, 0, 4095);
  b.writeCVOut(pressureCV, 4);
}




