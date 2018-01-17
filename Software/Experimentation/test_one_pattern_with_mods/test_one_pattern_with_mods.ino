//tests some manipulations of a single pattern
//prints the numerical output to the serial monitor also so you can see 
//what is happening.

#include <Betweener.h>
#include <Pattern.h>

Betweener b;

//one pattern for this test; trying out new-style constructor
int lengths[]={800,1000,400,600};  //lengths of the segments
Pattern p1(4, 20, lengths); //3 segments, dt = 100ms

void setup() {
  Serial.begin(115200);
  b.begin();

 
  //build pattern:  ramp first segment, stick with zeros
  //for second segment (leave it alone, basically)
  //do random numbers for third segment.
  p1.setRandMinMax(0,4095); //full range for random numbers

  p1.assignRampToSegment(0, 0,4000);
  p1.assignRandToSegment(2);
  p1.assignConstToSegment(3, 3000);


 
  Serial.println("pattern 1 (note, random parts will show as zeros)");
  p1.printToSerial();
 
}

float val;
int val1;
int scale;
int knob;
int selection;
int count = 1;

void loop() {


//uncomment this while loop to limit how much spews to the serial monitor for debugging
// while(count < 1000){
  val=p1.next();
  b.writeCVOut(val, 1);
  Serial.println(val);
  delay(20);

   if (Serial.available()){
    //grab first character that was input
    selection=Serial.read();
    //any other characters that were input are extras (e.g. newlines) so 
    //we read them just to flush them from the input buffer
    while(Serial.available()){
      Serial.read();
    }
   }
  if (selection == '1'){
    //hitting 1 switches it to reverse
    p1.reverse();
  }
  if (selection == '2'){
    //hitting 2 resets it to forward
    p1.forward();
  }
  if (selection == '3'){
    //hitting 3 holds where it is
    p1.hold();
  }
  if (selection == '4'){
    //hitting 4 restarts playback
    p1.unHold();
  }
  if (selection == '5'){
    //hitting 5 jumps to a particular sample
    p1.jumpTo(15);
  }
  if (selection == '6'){
    //hitting 6 jumps to a particular segment
    p1.jumpToSeg(2);
  
  }
  if (selection == '7'){
    //hitting 7 scales the dt for the whole thing 
    p1.scaleTime(2.5);
  }
  if (selection == '8'){
    //resets the dt to what we started with
    p1.reset();
  }
  if (selection == '9'){
    // this resets the time scaling only for one segment.  use 
    // keystroke 8 to reset this time too
    p1.scaleSegmentTime(0,2.5);
    
  }
   count ++;  //only using this if we're limiting the iterations with the while loop

// resetting this is critical so that we do not send pattern a gazillion repeating requests
   selection = 0;

// version for trying out using my midi foot pedal for time stretching:   
//   if(b.DINMIDI.read()){
//      if (b.DINMIDI.getData1() == 4){
//        val1=b.DINMIDI.getData2();
 
//        scale = map(val1, 0,127, 0,5);  
//        Serial.println(String("scale")+scale);
//        p1.scaleTime(scale);
//      }
//   }


//version that uses first Betweener knob for the same thing.
//this can be made much more fine-grained by using a floating point 
//mapping rather than the annoyingly int-based built-in map function
//  b.readKnobs();
//  knob=map(b.currentKnob1, 0, 1023, 0, 10);
//  p1.scaleTime(knob);
  
   
//  }  //end of optional while loop


  

}
