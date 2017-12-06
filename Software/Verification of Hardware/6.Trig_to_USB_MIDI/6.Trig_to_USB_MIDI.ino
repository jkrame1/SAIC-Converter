// Trigs to USB MIDI

/*
  Triggers all generate notes over USB MIDI
  *    -> 60
  **   -> 61
  ***  -> 62
  **** -> 63
*/

// CV to USB MIDI
#include <SPI.h>
#include <Bounce.h>

// the MIDI channel number to send messages
const int channel = 1;


// Create Bounce objects for each button.  The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
Bounce trig1 = Bounce(0, 5);  // 1 = 1 ms debounce time
Bounce trig2 = Bounce(3, 5);
Bounce trig3 = Bounce(5, 5);
Bounce trig4 = Bounce(4, 5);

void setup() {
  Serial.begin(115200);

  pinMode(0, INPUT_PULLUP);  //set internal pullup resistors for Trigger inputs
  pinMode(3, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(8, OUTPUT);        //indicate on pin 13 that a Trig has been recieved
}

void loop() {

  usbMIDI.read();

  trig1.update();
  trig2.update();
  trig3.update();
  trig4.update();

  // Check each button for "falling" edge.
  // Send a MIDI Note On message when each button presses
  // Update the trig inputs only upon changes.
  // falling = high (not pressed - voltage from pullup resistor)
  // to low (pressed - button connects pin to ground)

  if (trig1.fallingEdge()) {
    usbMIDI.sendNoteOn(60, 99, channel);  // 60 = C4
    digitalWrite(8, HIGH);
  }
  if (trig2.fallingEdge()) {
    usbMIDI.sendNoteOn(61, 99, channel);  // 61 = C#4
    digitalWrite(8, HIGH);
  }
  if (trig3.fallingEdge()) {
    usbMIDI.sendNoteOn(62, 99, channel);  // 62 = D4
    digitalWrite(8, HIGH);

  }
  if (trig4.fallingEdge()) {
    usbMIDI.sendNoteOn(63, 99, channel);  // 63 = D#4
    digitalWrite(8, HIGH);
  }

  // Check each button for "rising" edge
  // Send a MIDI Note Off message when each button releases
  // For many types of projects, you only care when the button
  // is pressed and the release isn't needed.
  // rising = low (pressed - button connects pin to ground)
  // to high (not pressed - voltage from pullup resistor)
  if (trig1.risingEdge()) {
    usbMIDI.sendNoteOff(60, 0, channel);  // 60 = C4
    digitalWrite(8, LOW);
  }
  if (trig2.risingEdge()) {
    usbMIDI.sendNoteOff(61, 0, channel);  // 61 = C#4
    digitalWrite(8, LOW);

  }
  if (trig3.risingEdge()) {
    usbMIDI.sendNoteOff(62, 0, channel);  // 62 = D4
    digitalWrite(8, LOW);

  }
  if (trig4.risingEdge()) {
    usbMIDI.sendNoteOff(63, 0, channel);  // 63 = D#4
    digitalWrite(8, LOW);

  }
}  
