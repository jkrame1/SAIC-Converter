
const int touchPin1 = 32;  // touch input pin that the electrode is attached to
const int touchPin2 = 25;  // touch input pin that the electrode is attached to
const int touchPin3 = 33;  // touch input pin that the electrode is attached to

int sensorValue1 = 0;        // value read from the touch tab
int sensorValue2 = 0;        // value read from the touch tab
int sensorValue3 = 0;        // value read from the touch tab

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  // read the analog in value:
  sensorValue1 = touchRead(touchPin1);
  sensorValue2 = touchRead(touchPin2);
  sensorValue3 = touchRead(touchPin3);


  // print the results to the Serial Monitor:
  Serial.print("sensor1 = ");
  Serial.println(sensorValue1);

  Serial.print("sensor2 = ");
  Serial.println(sensorValue2);

  Serial.print("sensor3 = ");
  Serial.println(sensorValue3);
  Serial.println();
  Serial.println();


  // wait 2 milliseconds before the next loop for the analog-to-digital
  // converter to settle after the last reading:
  delay(20);
}
