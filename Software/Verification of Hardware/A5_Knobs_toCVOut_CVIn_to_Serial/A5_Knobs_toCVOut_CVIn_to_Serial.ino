//Read the 4 knobs of the converter device using the 10-bit analogRead, 
//then convert the values from the 10-bit input range (0-1023)
//to the 12-bit output range (0-4015), then
//print the values to the 12-bit CV Outputs
//Read the CV inputs, print the values to the Serial Monitor

#include <SPI.h>

//One DAC is wired up to have chip select to Teensy digital pin 1
//the other to Teensy 2
int chip_select = 1;
int chip_select2 = 2;

//create variables to hold scaled knob readings
int scaled1 = -1;
int scaled2 = -1;
int scaled3 = -1;
int scaled4 = -1;

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

  SPI.setMOSI(7);
  SPI.setSCK(14);
  SPI.begin();
  
  Serial.begin(115200); //open serial port to receive values from knobs

}

void loop() {
//Read each knob and write its value to the Serial Monitor
  int  CurrentKnob1 = analogRead(12); //read knob 1 and store the reading in a value called "CurrentKnob1"
  scaled1 = map(CurrentKnob1, 0, 1023, 0, 4095);  //scale the value from a 10-bit value to a 12-bit value
  MCP4922_write(chip_select2, 1, scaled1); //Write the scaled value of knob 1 to OUTPUT 1


  int  CurrentKnob2 = analogRead(13);
  scaled2 = map(CurrentKnob2, 0, 1023, 0, 4095);
  MCP4922_write(chip_select, 0, scaled2);  //OUTPUT 2


  int  CurrentKnob3 = analogRead(11);
  scaled3 = map(CurrentKnob3, 0, 1023, 0, 4095);
  MCP4922_write(chip_select2, 0, scaled3); //OUTPUT 3


  int  CurrentKnob4 = analogRead(10);
  scaled4 = map(CurrentKnob4, 0, 1023, 0, 4095);
  MCP4922_write(chip_select, 1, scaled4);  //OUTPUT 4



  //Read each CV and write its value to the Serial Monitor
  int  CV1 = analogRead(7); //read knob 1 and store the reading in a value called "CurrentKnob1"
  Serial.print("1: ");                //print the number 1, a colon, and a space
  Serial.println(CV1);       //print the current value of knob 1

  int  CV2 = analogRead(6);
  Serial.print("2: ");
  Serial.println(CV2);

  int  CV3 = analogRead(3);
  Serial.print("3: ");
  Serial.println(CV3);

  int  CV4 = analogRead(2);
  Serial.print("4: ");
  Serial.println(CV4);

  delay(10); //wait for 100 milliseconds (.1s) so that we don't make too many readings and clog up the serial data line
}

//this is the DAC write function.  Pulls the chip select
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
