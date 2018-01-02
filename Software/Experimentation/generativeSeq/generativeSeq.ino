/*
   Generative Betweener Sequencer

   requires Betweener libary
   by Kyle Werle
    2017-12-31
    SAIC Art and Technology and Sound Departments
    Betweener Library by Kathryn Schaffer
    Hardware by Joseph Kramer
*/

/**SET LIBRARIES**/

#include <Betweener.h>
#include <Wire.h>

Betweener b;

/**SET VARIABLES**/

//  sequencer arrays
int CVsequence[16];
int gateSequence[16];

//  sequencer length
int CVseqL = 15; //0-15
int gateSeqL = 15;

//  position of sequencer
int CVseqCount = 0;
int gateSeqCount = 0;

/**SETUP**/

void setup() {
  Serial.begin(115200);
  b.begin();


  //  randomize seed based on ambient analog read
  randomSeed(analogRead(A17)*PI);

  //  init sequencers
  randomizeCVseq();
  randomizeGateSeq();

}

/**MAIN**/

void loop() {

  b.readAllInputs();

  //  clock input on trig 4 edge
  //  push CV + gate count forward 1
  clockCount();

  //  on trig 1 edge randomize CV sequencer
  if (b.trig1.fallingEdge()) {
    Serial.println("trig 1");
    randomizeCVseq();
  }

  //  on trig 2 edge randomize gate sequencer
  if (b.trig2.fallingEdge()) {
    Serial.println("trig 2");
    randomizeGateSeq();

  }

  //  on trig 3 edge set clock back to 0
  if (b.trig3.fallingEdge()) {
    Serial.println("trig 3");
    CVseqCount = 0;
    gateSeqCount = 0;
  }

  //  write CV and gate to DAC1 and DAC2
  b.writeCVOut(CVsequence[CVseqCount], 1);
  //Serial.println(CVsequence[CVseqCount]);
  b.writeCVOut(gateSequence[gateSeqCount], 2);
  //Serial.println(gateSequence[gateSeqCount]);

}

/**FUNCTIONS**/

//  step clock count forward on rising trigger edge
void clockCount() {

  if (b.trig4.fallingEdge()) {
    Serial.println("trig 4");
    if (CVseqCount < CVseqL) {
      CVseqCount++;
    } else {
      CVseqCount = 0;
    }

    // Serial.println(CVseqCount);

    if (gateSeqCount < gateSeqL) {
      gateSeqCount++;
    } else {
      gateSeqCount = 0;
    }
  }
}

//  randomizes CV sequencer
//  knob positions control output possibilities
void randomizeCVseq() {

  int CV0 = b.readKnobCV(1);
  int CV1 = b.readKnobCV(2);
  int CV2 = b.readKnobCV(3);
  int CV3 = b.readKnobCV(4);

  for (int i = 0; i <= 15; i++) {
    int sel;
    int r = random(4);

    if (r == 0) {
      sel = CV0;
    } else if (r == 1) {
      sel = CV1;
    } else if (r == 2) {
      sel = CV2;
    } else if (r == 3) {
      sel = CV3;
    }

    CVsequence[i] = sel;
  }
}

//  randomize gate sequencer
//  UPDATE TO INCLUDE DENSITY
void randomizeGateSeq() {

  for (int i = 0; i <= 15; i++) {
    float rF = random(2);
    if (rF == 1) {
      rF = 4095;
    }
    int rI = (int) rF;

    gateSequence[i] = rI;
  }
}

