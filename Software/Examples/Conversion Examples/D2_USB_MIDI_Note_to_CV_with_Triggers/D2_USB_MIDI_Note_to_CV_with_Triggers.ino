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
//Example Code by Joseph Kramer - 5 March 2018
///////////////////////////////////////////////////////////

#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages

void setup() {
  b.begin();  //start the Betweener

  Serial.begin(115200);

  usbMIDI.setHandleNoteOff(OnNoteOff);
  usbMIDI.setHandleNoteOn(OnNoteOn);
  usbMIDI.setHandleAfterTouch(OnAfterTouch);

  pinMode(8, OUTPUT); // Set Pin 8, attached to the Betweener LED, to an Output
}


void loop() {
  usbMIDI.read(); // USB MIDI receive
}



//NOTE ON - Sets a relative Pitch CV Out on Channel 1
//Gate HIGH on Channel 2 for any note with non-zero velocity
//Gate LOW for Note On messages with a value on zero (often used as Note Off)
//Velocity value is scaled and output on Channel 3
void OnNoteOn(byte channel, byte note, byte velocity) {
  if (channel == 10) {
    if (note == 35 || note == 36) {
      trigOut(1, velocity);
    }
    if (note == 37 || note  == 38) {
      trigOut(2, velocity);
    }

    if (note == 42) {
      trigOut(3, velocity);
    }
    if (note == 44 || note  == 46) {
      trigOut(4, velocity);
    }

  }


  else {
    int noteCV = map(note, 0, 127, 0, 4095); //turn noteON messages into a CV range
    b.writeCVOut(noteCV, 1); //When a note-on is received, write a CV value scaled to the note value

    if (velocity == 0) {
      b.writeCVOut(0, 2); //When a note-off is received, write a gate LOW on output 2
      digitalWrite(8, LOW); //turn LED off if no Gate is present
    }
    else {
      b.writeCVOut(4095, 2); //When a note-on is received, write a gate HIGH on output 2
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
    }

    int velocityCV = map(velocity, 0, 127, 10, 4095); //scale MIDI velocity values to 12bit range for CV output
    b.writeCVOut(velocityCV, 3); //Write velocity CV on channel 3 output
  }

}
//NOTE OFF - Sets Gate LOW on Channel 2 if a Note Off message is received
void OnNoteOff(byte channel, byte note, byte velocity) {
  b.writeCVOut(0, 2); //When a note-off is received, write a gate LOW on output 2
  digitalWrite(8, LOW); //Turn LED off when a Note Off is received
}


//AFTERTOUCH - CV Out on Channel 4
void OnAfterTouch(byte channel, byte pressure) {
  int pressureCV = map(pressure, 0, 127, 0, 4095);
  b.writeCVOut(pressureCV, 4);
}

void trigOut(byte out, int scale) {
  int val = map(scale, 0, 127, 0, 4095);
  switch (out) {

    case 1:
      b.writeCVOut(val, 1);
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
      delay(15);
      digitalWrite(8, LOW); //turn LED Off
      b.writeCVOut(0, 1);
      break;

    case 2:
      b.writeCVOut(val, 2);
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
      delay(15);
      digitalWrite(8, LOW); //turn LED Off
      b.writeCVOut(0, 2);
      break;

    case 3:
      b.writeCVOut(val, 3);
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
      delay(15);
      digitalWrite(8, LOW); //turn LED Off
      b.writeCVOut(0, 3);
      break;

    case 4:
      b.writeCVOut(val, 4);
      digitalWrite(8, HIGH); //turn LED On when a Note On message is received
      delay(15);
      digitalWrite(8, LOW); //turn LED Off
      b.writeCVOut(0, 4);
      break;

    default:
      break;
  }
}

