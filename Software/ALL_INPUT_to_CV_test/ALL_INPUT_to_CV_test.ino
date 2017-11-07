//CVin and Knobs are mapped to write CV
/*

  CV Inputs are mapped to corresponiding outputs
       ->  (PIN A2)
  **   ->  (PIN A3)
  ***  ->  (PIN A6)
  **** ->  (PIN A7)

  Knobs are mapped to corresponiding outputs
       ->  (PIN A12)
  **   ->  (PIN A13)
  ***  ->  (PIN A10)
  **** ->  (PIN A11)


  CV Outputs have the following Functions
       -> outputs scaled version of knob, input CV, or toggled full voltage for a trigger
  **   -> outputs scaled version of knob, input CV, or toggled full voltage for a trigger
  ***  -> outputs scaled version of knob, input CV, or toggled full voltage for a trigger
  **** -> outputs scaled version of knob, input CV, or toggled full voltage for a trigger

*/

//Includes code from Kathryn Schaffer, Joseph Kramer

// CV to USB MIDI
#include <SPI.h>


// the MIDI channel number to send messages
const int channel = 1;

const int knob1 = A12;
const int knob2 = A13;
const int knob3 = A10;
const int knob4 = A11;

const int CVin1 = A7;
const int CVin2 = A6;
const int CVin3 = A3;
const int CVin4 = A2;

//One DAC is wired up to have chip select to Teensy digital pin 1
//the other to Teensy 2
const int chip_select = 1;
const int chip_select2 = 2;

int in1;  //variable to read CV in
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
int scaledKnob1 = 0;
int scaledKnob2 = 0;
int scaledKnob3 = 0;
int scaledKnob4 = 0;


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

}



void loop() {

  //Read voltage on the CV Inputs and write the value to the CV Outputs
  in1 = analogRead(CVin1);
  scaledVal1 = map(in1, 0, 1023, 0, 4095);
  // MCP4922_write(chip_select, 1, scaledVal1);  //Write DAC1, channel 1
  Serial.print("CV1: ");
  Serial.println(scaledVal1);

  in2 = analogRead(CVin2);
  scaledVal2 = map(in2, 0, 1023, 0, 4095);
  // MCP4922_write(chip_select, 0, scaledVal2);  //Write DAC1, channel 2
  Serial.print("CV2: ");
  Serial.println(scaledVal2);

  in3 = analogRead(CVin3);
  scaledVal3 = map(in3, 0, 1023, 0, 4095);
  // MCP4922_write(chip_select2, 0, scaledVal3);  //Write DAC2, channel 1
  Serial.print("CV3: ");
  Serial.println(scaledVal3);

  in4 = analogRead(CVin4);
  scaledVal4 = map(in4, 0, 1023, 0, 4095);
  // MCP4922_write(chip_select2, 1, scaledVal4);  //Write DAC2, channel 2
  Serial.print("CV4: ");
  Serial.println(scaledVal4);



  //Read voltage on the Knobs and write the value to the CV Outputs
  in5 = analogRead(knob1);
  scaledKnob1 = map(in5, 0, 1023, 0, 4095);
  MCP4922_write(chip_select, 0, scaledKnob1);  //Write DAC1, channel 1
  Serial.print("knob1: ");
  Serial.println(scaledKnob1);

  in6 = analogRead(knob2);
  scaledKnob2 = map(in6, 0, 1023, 0, 4095);
  MCP4922_write(chip_select, 1, scaledKnob2);  //Write DAC1, channel 2
  Serial.print("knob2: ");
  Serial.println(scaledKnob2);

  in7 = analogRead(knob3);
  scaledKnob3 = map(in7, 0, 1023, 0, 4095);
  MCP4922_write(chip_select2, 0, scaledKnob3);  //Write DAC2, channel 1
  Serial.print("knob3: ");
  Serial.println(scaledKnob3);

  in8 = analogRead(knob4);
  scaledKnob4 = map(in8, 0, 1023, 0, 4095);
  MCP4922_write(chip_select2, 1, scaledKnob4);  //Write DAC2, channel 2
  Serial.print("knob4: ");
  Serial.println(scaledKnob4);


  delay(10);

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

void DAC_Write(int chan, int value) {
  if (chan == 1) {
    byte low = value & 0xff;
    byte high = (value >> 8) & 0x0f;
    byte dac = (0 & 1) << 7;
    digitalWrite(1, LOW);
    SPI.transfer(dac | 0x30 | high);
    SPI.transfer(low);
    digitalWrite(1, HIGH);
  }
  if (chan == 2) {
    byte low = value & 0xff;
    byte high = (value >> 8) & 0x0f;
    byte dac = (1 & 1) << 7;
    digitalWrite(1, LOW);
    SPI.transfer(dac | 0x30 | high);
    SPI.transfer(low);
    digitalWrite(1, HIGH);
  }

  if (chan == 3) {
    byte low = value & 0xff;
    byte high = (value >> 8) & 0x0f;
    byte dac = (0 & 1) << 7;
    digitalWrite(2, LOW);
    SPI.transfer(dac | 0x30 | high);
    SPI.transfer(low);
    digitalWrite(2, HIGH);
  }

  if (chan == 4) {
    byte low = value & 0xff;
    byte high = (value >> 8) & 0x0f;
    byte dac = (1 & 1) << 7;
    digitalWrite(2, LOW);
    SPI.transfer(dac | 0x30 | high);
    SPI.transfer(low);
    digitalWrite(2, HIGH);
  }
}






