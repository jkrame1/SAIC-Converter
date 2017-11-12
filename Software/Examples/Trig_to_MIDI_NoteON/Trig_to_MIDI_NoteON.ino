#include <Betweener.h>

Betweener b;

// the MIDI channel number to send messages
const int channel = 1;


void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}


void loop() {
  // ask the betweener to read all its inputs
  b.readAllInputs();

  // Check each button for "falling" edge.
  // Send a MIDI Note On message when each button presses
  // Update the triggers only upon changes.
  // falling = high (not pressed - voltage from pullup resistor)
  //           to low (pressed - button connects pin to ground)
  if (b.trig1.fallingEdge()) {
    usbMIDI.sendNoteOn(60, 99, channel);  // 60 = C4
  }

  if (b.trig2.fallingEdge()) {
    usbMIDI.sendNoteOn(62, 99, channel);  // 62 = D4
  }

  if (b.trig3.fallingEdge()) {
    usbMIDI.sendNoteOn(64, 99, channel);  // 64 = E4
  }

  if (b.trig4.fallingEdge()) {
    usbMIDI.sendNoteOn(69, 111, channel);  // 69 = A5
  }


  //SEND MIDI NoteOFF Messages for rising edge on triggers
  if (b.trig1.risingEdge()) {
    usbMIDI.sendNoteOn(60, 0, channel);  // 60 = C4
  }

  if (b.trig2.risingEdge()) {
    usbMIDI.sendNoteOn(62, 0, channel);  // 62 = D4
  }

  if (b.trig3.risingEdge()) {
    usbMIDI.sendNoteOn(64, 0, channel);  // 64 = E4
  }

  if (b.trig4.risingEdge()) {
    usbMIDI.sendNoteOn(69, 0, channel);  // 69 = A5
  }


  // MIDI Controllers should discard incoming MIDI messages.
  // http://forum.pjrc.com/threads/24179-Teensy-3-Ableton-Analog-CC-causes-midi-crash
  while (usbMIDI.read()) {
    // ignore incoming messages
  }
}
