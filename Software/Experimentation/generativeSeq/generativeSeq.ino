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

Betweener b;

/**SET VARIABLES**/

//  sequencer arrays
int CVsequence1[32];
int CVsequence2[32];
int chaosSequence1[32];
int chaosSequence2[32];

//  sequencer length
int seqL = 31; //0-31

//  position of sequencer
int seqCount = 0;

/**SETUP**/

void setup() {
  //  serial debugger
  //Serial.begin(115200);

  //  init betweener library
  b.begin();

  //  randomize seed based on ambient analog read
  randomSeed(analogRead(A17)*PI);

  //  init sequencers
  randomizeCVseq();
  randomizeChaosSeq();

}

/**MAIN**/

void loop() {

  b.readTriggers();

  //  clock input on trig 4 edge
  //  push sequencer count forward 1
  clockCount();

  //  on trig 1 edge randomize CV sequencer
  if (b.trig1.fell()) {
    randomizeCVseq();
  }

  //  on trig 2 edge randomize chaos sequencer
  if (b.trig2.fell()) {
    randomizeChaosSeq();

  }

  //  on trig 3 edge set clock back to 0
  if (b.trig3.fell()) {
    seqCount = 0;

  }
}

/**FUNCTIONS**/

//  step clock count forward on rising trigger edge
void clockCount() {

  if (b.trig4.fell()) {
    if (seqCount < seqL) {
      seqCount++;
    } else {
      seqCount = 0;
    }

    //  write CV and gate to DAC
    b.writeCVOut(CVsequence1[seqCount], 1);
    b.writeCVOut(CVsequence2[seqCount], 2);
    b.writeCVOut(chaosSequence1[seqCount], 3);
    b.writeCVOut(chaosSequence2[seqCount], 4);
  }
}

//  randomizes CV sequencer
//  knob positions control output possibilities
void randomizeCVseq() {

  int CV0 = b.readKnobCV(1);
  int CV1 = b.readKnobCV(2);
  int CV2 = b.readKnobCV(3);
  int CV3 = b.readKnobCV(4);

  for (int i = 0; i <= seqL; i++) {
    int sel = 0;
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

    CVsequence1[i] = sel;
  }

  for (int i = 0; i <= seqL; i++) {
    int sel = 0;
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

    CVsequence2[i] = sel;
  }
}

//  randomize gate sequencer
//  UPDATE TO INCLUDE DENSITY
void randomizeChaosSeq() {

  for (int i = 0; i <= seqL; i++) {
    int r = random(4095);
    chaosSequence1[i] = r;
  }
  
  for (int i = 0; i <= seqL; i++) {
    int r = random(4095);
    chaosSequence2[i] = r;
  }
}
