#include <Betweener.h>

int val=0;
Betweener b;

void setup() {
  Serial.begin(115200);  
}

void loop() {

 // ask the betweener to read all its inputs
 b.readAllInputs();

 Serial.println(b.currentKnob1);
 Serial.println(b.currentKnob2);
 Serial.println(b.currentKnob3);
 Serial.println(b.currentKnob4);


  if (b.trig3.risingEdge()){
  // Serial.println("rising edge");
  }

  if (b.trig3.fallingEdge()){
  // Serial.println("falling edge");
  }


// Serial.println(b.currentCV4);

 //grab the current value from CV input 1
 val=b.currentCV3;
// Serial.println(val);

//send unscaled values right back out on all channels
 b.writeCVOut(b.currentKnob1,1);
 b.writeCVOut(b.currentKnob2,2);
 b.writeCVOut(b.currentKnob3,3);
 b.writeCVOut(b.currentKnob4,4);


}
