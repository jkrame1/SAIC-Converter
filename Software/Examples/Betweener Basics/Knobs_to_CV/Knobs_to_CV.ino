/*


   This code reads the Betweener Knobs and sends
   correspondnig scaled CV out


*/


//include the Betweener library
#include <Betweener.h>

//make a Betweener object. anytime you want to talk to the Betweener
//using the library, you will start by using the name "b."
Betweener b;

void setup() {
  //the Betweener begin function is necessary before it will do anything
  b.begin();

}

void loop() {

  //these lines read each knob, scale the value, and write a CV
  b.writeCVOut(b.readKnobCV(1), 1);
  b.writeCVOut(b.readKnobCV(2), 2);
  b.writeCVOut(b.readKnobCV(3), 3);
  b.writeCVOut(b.readKnobCV(4), 4);

  delay(50);


}
