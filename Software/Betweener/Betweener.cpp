//
//  Betweener.cpp
//  
//
//  Created by kathryn schaffer on 10/23/17.
//
//

#include "Betweener.h"



//Constructor.  This code is run when you make a Betweener object.
Betweener::Betweener(void){
    
    
    //All of the upper-case "variables" here are actually
    //defined in preprocessor directives in the .h file.
    //They cannot be modified at run time.
    
    //These are the settings needed to get the SPI working
    //for the DACs to create CV out
    pinMode(DAC_CHIP_SELECT1, OUTPUT);
    digitalWrite(DAC_CHIP_SELECT1, HIGH);
    pinMode(DAC_CHIP_SELECT2, OUTPUT);
    digitalWrite(DAC_CHIP_SELECT2, HIGH);
   
    //Start SPI
    SPI.begin();
    SPI.setMOSI(SPI_MOSI);  //use alternate SPI MOSI
    SPI.setSCK(SPI_SCK);  //use alternate SPI SCK

    //Here, we tell Teensy what to do with those pins
    pinMode(TRIGGER_INPUT1, INPUT_PULLUP);  //set internal pullup resistors for Trigger inputs
    pinMode(TRIGGER_INPUT2, INPUT_PULLUP);
    pinMode(TRIGGER_INPUT3, INPUT_PULLUP);
    pinMode(TRIGGER_INPUT4, INPUT_PULLUP);
  
    
    // Create Bounce objects for each button.  The Bounce object
    // automatically deals with contact chatter or "bounce", and
    // it makes detecting changes very simple.
    // note that bounce_ms is set to a default in the .h file

    trig1.attach(TRIGGER_INPUT1);
    trig1.interval(bounce_ms);
    
    trig2.attach(TRIGGER_INPUT2);
    trig2.interval(bounce_ms);
    
    trig3.attach(TRIGGER_INPUT3);
    trig3.interval(bounce_ms);
    
    trig4.attach(TRIGGER_INPUT4);
    trig4.interval(bounce_ms);
 
    //starting defaults for the values representing analog inputs and knob setttings
    currentCV1=-1;
    currentCV2=-1;
    currentCV3=-1;
    currentCV4=-1;

    currentKnob1=-1;
    currentKnob2=-1;
    currentKnob3=-1;
    currentKnob4=-1;
    
    //Start the ticker timers
    msecTickerCVRead = 0;
    msecTickerTrigger = 0;
    msecTickerMIDI = 0;
    
}

void Betweener::readTriggers(void){
    
    trig1.update();
    trig2.update();
    trig3.update();
    trig4.update();
    
}


void Betweener::readCVInputs(void){
    
    if (msecTickerCVRead >= analog_read_delta){
        msecTickerCVRead = 0;  //reset
        currentCV1 = analogRead(ANALOG1);
        currentCV2 = analogRead(ANALOG2);
        currentCV3 = analogRead(ANALOG3);
        currentCV4 = analogRead(ANALOG4);
    }
}


void Betweener::readKnobs(void){
    // do we want the /8 here??  do we also want a ticker?
    currentKnob1 = analogRead(KNOB1);
    currentKnob2 = analogRead(KNOB2);
    currentKnob3 = analogRead(KNOB3);
    currentKnob4 = analogRead(KNOB4);
}

void Betweener::readUsbMIDI(void) {
    usbMIDI.read();
    
}

void Betweener::readAllInputs(void){
    readTriggers();
    readCVInputs();
    readKnobs();
    readUsbMIDI();
}

void Betweener::MCP4922_write(int cs_pin, byte dac, int value){
    byte low = value & 0xff;
    byte high = (value >> 8) & 0x0f;
    dac = (dac & 1) << 7;
    digitalWrite(cs_pin, LOW);
    SPI.transfer(dac | 0x30 | high);
    SPI.transfer(low);
    digitalWrite(cs_pin, HIGH);
}


void Betweener::writeCVOut(int value, int cvout){
    int cs_pin;
    byte dac;
    switch (cvout){
        case 1:
            cs_pin = CVIN1_CHIP_SELECT;
            dac = CVIN1_DAC_CHANNEL;
            break;
        case 2:
            cs_pin = CVIN2_CHIP_SELECT;
            dac = CVIN2_DAC_CHANNEL;
            break;
        case 3:
            cs_pin = CVIN3_CHIP_SELECT;
            dac = CVIN3_DAC_CHANNEL;
            break;
        case 4:
            cs_pin = CVIN4_CHIP_SELECT;
            dac=CVIN4_DAC_CHANNEL;
            break;
        //need to stick in a default if there is a weird value
    }
    
    //should put in some idiot checks here that the value is reasonable...
    
    MCP4922_write(cs_pin, dac,value);
}

