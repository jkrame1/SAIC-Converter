// CV to USB MIDI to CV
/*

  Triggers all generate Notes over USB MIDI
  *    -> 60  (PIN D4)
  **   -> 61  (PIN D5)
  ***  -> 62  (PIN D3)
  **** -> 63  (PIN D0)

  CV Inputs are mapped to CC values
  *    -> 10  (PIN A2)
  **   -> 11  (PIN A3)
  ***  -> 91  (PIN A6)
  **** -> 93  (PIN A7)

  Knobs are mapped CC Values
  *    -> 20  (PIN A12)
  **   -> 21  (PIN A13)
  ***  -> 22  (PIN A10)
  **** -> 23  (PIN A11)


  CV Outputs have the following Functions
  *    -> outputs only note#0, DAC value = to velocity scaled to 12bit, responds to note off
  **   -> outputs only note#1, DAC value = to velocity scaled to 12bit, responds to note off
  ***  -> outputs a voltage read on USB MIDI coming in on CC#22
  **** -> outputs a pitch relative to...???

*/

//Includes code from Kathryn Schaffer, Joseph Kramer, and examples from Paul Stoffregen:
/* Buttons to USB MIDI Example
   You must select MIDI from the "Tools > USB Type" menu
   To view the raw MIDI data on Linux: aseqdump -p "Teensy MIDI"
   This example code is in the public domain.
*/

// CV to USB MIDI
#include <SPI.h>
#include <Bounce.h>

// the MIDI channel number to send messages
const int channel = 1;

// Set the MIDI continuous controller for each analog input:
// CV Input Jacks
const int controllerA0 = 10; // 10 = pan position
const int controllerA1 = 11; // 11 = volume/expression
const int controllerA2 = 91; // 91 = reverb level
const int controllerA3 = 93; // 93 = chorus level
// Controller Knobs
const int controllerA4 = 20; // CCs 20 - 31 are undefined
const int controllerA5 = 21;
const int controllerA6 = 22;
const int controllerA7 = 23;

int in1;  //variable to read CV in Chan1
int in2;
int in3;
int in4;
int in5;
int in6;
int in7;
int in8;


int scaledVal1 = 0;  //variable to remap a 10bit value to a 12bit value
int scaledVal2 = 0;
int scaledVal3 = 0;
int scaledVal4 = 0;
int scaledVal5 = 0;
int scaledVal6 = 0;
int scaledVal7 = 0;
int scaledVal8 = 0;

// Create Bounce objects for each button.  The Bounce object
// automatically deals with contact chatter or "bounce", and
// it makes detecting changes very simple.
Bounce trig1 = Bounce(0, 5);  // 1 = 1 ms debounce time
Bounce trig2 = Bounce(3, 5);
Bounce trig3 = Bounce(4, 5);
Bounce trig4 = Bounce(5, 5);

//One DAC is wired up to have chip select to Teensy digital pin 1
//the other to Teensy 2
int chip_select = 1;
int chip_select2 = 2;

//sets the chip select to high to start
//and starts communicating over normal SPI pins
//which are pins 13 (clock) and 11 (MOSI)
//unless we set those to the alternate options
// 14 (clock) and 7 (MOSI), which is what we
//want if we are using the audio shield too

void setup() {
  pinMode(chip_select, OUTPUT);
  digitalWrite(chip_select, HIGH); // HIGH);
  pinMode(chip_select2, OUTPUT);
  digitalWrite(chip_select2, HIGH); // HIGH);

  SPI.begin();
  SPI.setMOSI(7);  //use alternate SPI MOSI
  SPI.setSCK(14);  //use alternate SPI SCK

  Serial.begin(115200);

  usbMIDI.setHandleNoteOff(OnNoteOff);  //set USB MIDI read for note off
  usbMIDI.setHandleNoteOn(OnNoteOn);   //set USB MIDI read for note on
  usbMIDI.setHandleControlChange(OnControlChange);  //set USB MIDI read function for CC

  pinMode(0, INPUT_PULLUP);  //set internal pullup resistors for Trigger inputs
  pinMode(3, INPUT_PULLUP);
  pinMode(4, INPUT_PULLUP);
  pinMode(5, INPUT_PULLUP);
  //  pinMode(13, OUTPUT);        //indicate on pin 13 that a Trig has been recieved
}



// store previously sent values, to detect changes
int previousA0 = -1;
int previousA1 = -1;
int previousA2 = -1;
int previousA3 = -1;

int previousA4 = -1;
int previousA5 = -1;
int previousA6 = -1;
int previousA7 = -1;

elapsedMillis msec = 0;



void loop() {

  usbMIDI.read();

    // only check the analog inputs 50 times per second,
    // to prevent a flood of MIDI messages
  if (msec >= 20) {
    msec = 0;
    //Read 4 CV Inputs and then 4 knobs
    int n0 = analogRead(3) / 8;  //0-1023 divided by 8 scales the 10bits down to 7bits (0-127)
    int n1 = analogRead(2) / 8;
    int n2 = analogRead(6) / 8;
    int n3 = analogRead(7) / 8;
    int n4 = analogRead(12) / 8;
    int n5 = analogRead(13) / 8;
    int n6 = analogRead(10) / 8;
    int n7 = analogRead(11) / 8;

    // only transmit MIDI messages if analog input changed
    if (n0 != previousA0) {
      usbMIDI.sendControlChange(controllerA0, n0, channel);
      previousA0 = n0;
    }
    if (n1 != previousA1) {
      usbMIDI.sendControlChange(controllerA1, n1, channel);
      previousA1 = n1;
    }
    if (n2 != previousA2) {
      usbMIDI.sendControlChange(controllerA2, n2, channel);
      previousA2 = n2;
    }
    if (n3 != previousA3) {
      usbMIDI.sendControlChange(controllerA3, n3, channel);
      previousA3 = n3;
    }
    if (n4 != previousA4) {
      usbMIDI.sendControlChange(controllerA4, n4, channel);
      previousA4 = n4;
    }
    if (n5 != previousA5) {
      usbMIDI.sendControlChange(controllerA5, n5, channel);
      previousA5 = n5;
    }
    if (n6 != previousA6) {
      usbMIDI.sendControlChange(controllerA6, n6, channel);
      previousA6 = n6;
    }
    if (n7 != previousA7) {
      usbMIDI.sendControlChange(controllerA7, n7, channel);
      previousA7 = n7;
    }

  }

  /*
    //Read voltage on the CV Inputs and write the value to the CV Outputs
      in1 = analogRead(3);
    //    Serial.print("CV1: ");
    //    Serial.println(scaledVal1);
      scaledVal1 = map(in1, 1, 1023, 0, 4095);
      MCP4922_write(chip_select, 1, scaledVal1);  //Write DAC1, channel 1

      in2 = analogRead(2);
    //    Serial.print("CV2: ");
    //    Serial.println(scaledVal2);
      scaledVal2 = map(in2, 1, 1023, 0, 4095);
      MCP4922_write(chip_select, 0, scaledVal2);  //Write DAC1, channel 2

      in3 = analogRead(6);
    //    Serial.print("CV3: ");
    //    Serial.println(scaledVal3);
      scaledVal3 = map(in3, 1, 1023, 0, 4095);
      MCP4922_write(chip_select2, 0, scaledVal3);  //Write DAC2, channel 1

      in4 = analogRead(7);
    //    Serial.print("CV4: ");
    //    Serial.println(scaledVal4);
      scaledVal4 = map(in4, 1, 1023, 0, 4095);
      MCP4922_write(chip_select2, 1, scaledVal4);  //Write DAC2, channel 2
  */

  trig1.update();
  trig2.update();
  trig3.update();
  trig4.update();

    // Check each button for "falling" edge.
    // Send a MIDI Note On message when each button presses
    // Update the Joystick buttons only upon changes.
    // falling = high (not pressed - voltage from pullup resistor)
    // to low (pressed - button connects pin to ground)
  if (trig1.fallingEdge()) {
    usbMIDI.sendNoteOn(60, 99, channel);  // 60 = C4
    //  MCP4922_write(chip_select, 1, 4000);  //Write DAC1, channel 1, High
  }
  if (trig2.fallingEdge()) {
    usbMIDI.sendNoteOn(61, 99, channel);  // 61 = C#4
    //  MCP4922_write(chip_select, 0, 4000);  //Write DAC1, channel 2
  }
  if (trig3.fallingEdge()) {
    usbMIDI.sendNoteOn(62, 99, channel);  // 62 = D4
    //  MCP4922_write(chip_select2, 0, 4000);  //Write DAC2, channel 1
  }
  if (trig4.fallingEdge()) {
    usbMIDI.sendNoteOn(63, 99, channel);  // 63 = D#4
    //  MCP4922_write(chip_select2, 1, 4000);  //Write DAC2, channel 2
  }

    // Check each button for "rising" edge
    // Send a MIDI Note Off message when each button releases
    // For many types of projects, you only care when the button
    // is pressed and the release isn't needed.
    // rising = low (pressed - button connects pin to ground)
    // to high (not pressed - voltage from pullup resistor)
  if (trig1.risingEdge()) {
    usbMIDI.sendNoteOff(60, 0, channel);  // 60 = C4
  }
  if (trig2.risingEdge()) {
    usbMIDI.sendNoteOff(61, 0, channel);  // 61 = C#4
  }
  if (trig3.risingEdge()) {
    usbMIDI.sendNoteOff(62, 0, channel);  // 62 = D4
  }
  if (trig4.risingEdge()) {
    usbMIDI.sendNoteOff(63, 0, channel);  // 63 = D#4
  }
}


    //this is the write function.  Pulls the chip select
    //low in each write cycle and then sets it high again.
    //the inscrutable stuff here is to get the right
    //byte format for the chip
void MCP4922_write(int cs_pin, byte dac, int value) {
  byte low = value & 0xff;
  byte high = (value >> 8) & 0x0f;
  dac = (dac & 1) << 7;
  digitalWrite(cs_pin, LOW);
  SPI.transfer(dac | 0x30 | high);
  SPI.transfer(low);
  digitalWrite(cs_pin, HIGH);
}



    //this is a function to read MIDI notes in and toggle the voltage on chan4
void OnNoteOn(byte channel, byte note, byte velocity) {
  Serial.println(note);
  if (note > 1) { //only writes if note is greater than 0
    // Serial.println(note);
    //scale MIDI note to relative 12bit output voltage (the 2 here lets the lowest note = 0V)
    int cvOut = map(note, 2, 127, 0, 4095);
    MCP4922_write(chip_select2, 1, cvOut);  //Write channel 4, High
  }
  else if (note == 1) {
    int tapLevel = map(velocity, 0, 127, 0, 4095); //scale velocity
    Serial.println(tapLevel);
    MCP4922_write(chip_select, 1, tapLevel);  //Write channel 2 at a level scaled to Velocity
  }
  else if (note == 0) {
    int tapLevel = map(velocity, 0, 127, 0, 4095); //scale velocity
    MCP4922_write(chip_select, 0, tapLevel);  //Write channel 1 at a level scaled to Velocity
    //delay(10);
  }
}


    //this is a function to set the voltage on ch1 back to zero on noteOff
void OnNoteOff(byte channel, byte note, byte velocity) {
  if (note == 1) {
    MCP4922_write(chip_select, 1, velocity);  //Write channel 2, Low
  }
  if (note == 0) {
    MCP4922_write(chip_select, 0, velocity);  //Write channel 1, Low
  }
}



    //This function reads MIDI CC #22 on any channel
    //It scales the incomeing value and writes directly to CV out 3
void OnControlChange(byte channel, byte control, byte value) {
  if (control = 22) {
    int newValue = map(value, 0, 127, 0, 4095);
    MCP4922_write(chip_select2, 0, newValue);  //Write to output 3
  }

  Serial.print("Control Change, ch=");
  Serial.print(channel, DEC);
  Serial.print(", control=");
  Serial.print(control, DEC);
  Serial.print(", value=");
  Serial.print(value, DEC);
  Serial.println();
}
