//Read the 4 knobs of the converer device using analogRead, then
//print the values to the Serial Monitor

void setup() {

  Serial.begin(115200); //open serial port to receive values from knobs

}

void loop() {

  //Read each knob and write its value to the Serial Monitor
  int  CurrentKnob1 = analogRead(12); //read knob 1 and store the reading in a value called "CurrentKnob1"
  Serial.print("1: ");                //print the number 1, a colon, and a space
  Serial.println(CurrentKnob1);       //print the current value of knob 1

  int  CurrentKnob2 = analogRead(13);
  Serial.print("2: ");
  Serial.println(CurrentKnob2);

  int  CurrentKnob3 = analogRead(11);
  Serial.print("3: ");
  Serial.println(CurrentKnob3);

  int  CurrentKnob4 = analogRead(10);
  Serial.print("4: ");
  Serial.println(CurrentKnob4);

  delay(100); //wait for 100 milliseconds (.1s) so that we don't make too many readings and clog up the serial data line
}
