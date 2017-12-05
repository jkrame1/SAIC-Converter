#include <Betweener.h>

int CVout1 = 0;
int CVout2 = 0;
int CVout3 = 0;
int CVout4 = 0;
int val = 0;

Betweener b;

void setup() {
  Serial.begin(115200);  
}

void loop() {

 // ask the betweener to read all its inputs
 b.readAllInputs();

CVout1 = map(b.currentKnob1,0, 1023, 0, 4095);
CVout2 = map(b.currentKnob2,0, 1023, 0, 4095);
CVout3 = map(b.currentKnob3,0, 1023, 0, 4095);
CVout4 = map(b.currentKnob4,0, 1023, 0, 4095);

 Serial.println(CVout1);
 Serial.println(CVout2);
 Serial.println(CVout3);
 Serial.println(CVout4);

  if (b.trig3.risingEdge()){
  // Serial.println("rising edge");
  }

  if (b.trig3.fallingEdge()){
    for(int i = 0; i <= 4095; i ++){
  b.writeCVOut(CVout1,i);
  }
  // Serial.println("falling edge");
  }


// Serial.println(b.currentCV4);

 //grab the current value from CV input 1
 val=b.currentCV3;
// Serial.println(val);

//send unscaled values right back out on all channels
 b.writeCVOut(CVout1,1);
 b.writeCVOut(CVout2,2);
 b.writeCVOut(CVout3,3);
 b.writeCVOut(CVout4,4);


}

void env(){
  for(int i = 0; i <= 4095; i ++){
  b.writeCVOut(i,1);
  }
}

