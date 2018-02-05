///////////////////////////////////////////////////////////
//USB_MIDI_Note_to_CV
//
//This code receives MIDI over USB from a computer and generates Control Voltages
//on the four Betweener outputs. 
//  1 Note to CV - this is NOT 1v/oct - just a rough mapping of all notes to full CV range
//  2 Gate - High for Note On Messages and Low for Note Off messages (or velocity of 0)
//  3 Velocity CV - very wide range, can be remapped in NOTE ON function below
//  4 Aftertouch CV - mapped for full voltage output range
//
//includes portions of PJRC Teensy MIDI examples
//Uses the Betweener Library by Kathryn Schaffer
//
//Example Code by Joseph Kramer - 5 Feb 2018
///////////////////////////////////////////////////////////

#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;


void setup() {
  b.begin();
  Serial.begin(115200);
  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleAfterTouch(OnAfterTouch);
}


void loop() {
  usbMIDI.read(); // USB MIDI receive
}



//NOTE ON - Sets a relative Pitch CV Out on Channel 1
//Gate HIGH on Channel 2 for any note with non-zero velocity
//Gate LOW for Note On messages with a value on zero (often used as Note Off)
//Velocity value is scaled and output on Channel 3
void OnNoteOn(byte channel, byte note, byte velocity) {
  int noteCV = map(note, 0, 127, 0, 4095); //turn noteON messages into a CV range
  b.writeCVOut(noteCV, 1); //When a note-on is received, write a CV value scaled to the note value

  if (velocity == 0) {
    b.writeCVOut(0, 2); //When a note-off is received, write a gate LOW on output 2
  }
  else {
    b.writeCVOut(4095, 2); //When a note-on is received, write a gate HIGH on output 2
    digitalWrite(8, HIGH);
  }

  int velocityCV = map(velocity, 0, 127, 10, 4095); //scale MIDI velocity values to 12bit range for CV output
  b.writeCVOut(velocityCV, 3); //Write velocity CV on channel 3 output
}


//NOTE OFF - Sets Gate LOW on Channel 2 if a Note Off message is received
void OnNoteOff(byte channel, byte note, byte velocity) {
  b.writeCVOut(0, 2); //When a note-off is received, write a gate LOW on output 2
  digitalWrite(8, LOW);
}


//AFTERTOUCH - CV Out on Channel 4
void OnAfterTouch(byte channel, byte pressure) {
  int pressureCV = map(pressure, 0, 127, 0, 4095);
  b.writeCVOut(pressureCV, 4);
}



