//tests outputting four independent patterns to Betweener CV outs

#include <Betweener.h>
#include <Pattern.h>

Betweener b;

//one pattern per CV out.  Somewhat arbitrary choices for testing
Pattern p1(3, 6, 500); //3 seconds, 6 segments, dt of 500ms
Pattern p2(2, 6, 50); //1 second, 5 segments, dt of 50ms
Pattern p3(2, 1, 50); //2 seconds, 1 segment, dt of 50ms
Pattern p4(3,8,100);  //3 seconds, 8 segments, dt 100ms

void setup() {
  Serial.begin(115200);
  b.begin();

  //build the patterns.  In the future we can have a bunch of 
  //preset and time-scalable patterns built into libraries
  
  //pattern 1:  alternate values 0 and 1000 every 500ms for 3 sec
  p1.assignConstToSegment(1, 1000);
  p1.assignConstToSegment(3, 1000);
  p1.assignConstToSegment(5, 1000);

  //pattern 2:  a bunch of repeating ramps from 0 to CV out max (4095)
  p2.assignRampToSegments(0,5,0,4095);

  //pattern 3:  a single ramp down from max to min
  p3.assignRampToSegment(0,4095,0);

  //pattern 4:  a set of constant values
  float steps[8]={1000,2000,3000,0,1000,2000,3000,0};
  p4.fillSteps(steps);

  //printout for reassurance:
  Serial.println("pattern 1");
  p1.printToSerial();
  Serial.println("pattern 2");
  p2.printToSerial();
  Serial.println("pattern 3");
  p3.printToSerial();
 // Serial.println("pattern 4");
 // p4.printToSerial();
  

}

void loop() {
  b.writeCVOut(p1.next(), 1);
  b.writeCVOut(p2.next(),2);
  b.writeCVOut(p3.next(),3);
  b.writeCVOut(p4.next(),4);
  delay(20); //just seems like a good idea to keep things from going insane??

}
