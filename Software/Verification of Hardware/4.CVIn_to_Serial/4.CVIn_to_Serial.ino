//Read the 4 CV Inputs of the converer device using analogRead, then
//print the values to the Serial Monitor

void setup() {

  Serial.begin(115200); //open serial port to receive values from knobs

}

void loop() {

  //Read each CV Input and write its value to the Serial Monitor
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
