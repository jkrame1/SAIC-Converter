//
//  Betweener.cpp
//  
//  See the Betweener.h file for lots of comments and information
//  for making sense of what this is.
//
//
//  Created by kathryn schaffer on 10/23/17.
//


//the functions and many variables in this file were all
//declared in Betweener.h, so we include it
#include "Betweener.h"

//Now, below, we have the code implementing all the functions
//(a.k.a. methods) of the Betweener class.
//The Betweener:: syntax specifies to the compiler that these
//functions live in the "Betweener namespace" which means that
//they will not conflict with other functions associated with
//other classes that have similar names.

//Constructor.  This code is run when you make a Betweener object.
Betweener::Betweener(void){
    
    //starting defaults for the values representing analog inputs and knob setttings
    currentCV1=-1;
    currentCV2=-1;
    currentCV3=-1;
    currentCV4=-1;
    
    currentKnob1=-1;
    currentKnob2=-1;
    currentKnob3=-1;
    currentKnob4=-1;


}
    

void Betweener::begin(void){
    //this does all the real setup stuff
    
    //Note that al of the upper-case "variables" here are actually
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
 

    
    //Start the ticker timers
    msecTickerCVRead = 0;
    msecTickerTrigger = 0;
    msecTickerMIDI = 0;

    
    //If we are using DIN MIDI I/O we need some setup:
#ifdef DODINMIDI
    Serial2.setRX(DINMIDIIN);
    Serial2.setTX(DINMIDIOUT);
    DINMIDI.begin(MIDI_CHANNEL_OMNI);  //monitor all input channels
#endif
    
    //If we are using digipots, we need to be running the Wire library
#ifdef DODIGIPOTS
    Wire.begin();
#endif
  
}





void Betweener::readTriggers(void){
    //The bounce library has a function update() that is the
    //main read function
    trig1.update();
    trig2.update();
    trig3.update();
    trig4.update();
    
}


void Betweener::readCVInputs(void){
    //we could put stuff in hear to limit the read
    //rate, but right now we'll leave that to the sketch
        currentCV1 = analogRead(ANALOG1);
        currentCV2 = analogRead(ANALOG2);
        currentCV3 = analogRead(ANALOG3);
        currentCV4 = analogRead(ANALOG4);

}


void Betweener::readKnobs(void){
 
    currentKnob1 = analogRead(KNOB1);
    currentKnob2 = analogRead(KNOB2);
    currentKnob3 = analogRead(KNOB3);
    currentKnob4 = analogRead(KNOB4);
}

void Betweener::readUsbMIDI(void) {
    //with each read, the usbMidi object
    //will store whatever messages it most recently received
    usbMIDI.read();
    
}


#ifdef DODINMIDI
void Betweener::readDINMIDI(void){

        DINMIDI.read();
}
#endif





void Betweener::readAllInputs(void){
    //run the previous four functions all in sequence
    readTriggers();
    readCVInputs();
    readKnobs();
    readUsbMIDI();
#ifdef DODINMIDI
    readDINMIDI();
#endif
}

void Betweener::readTrigger(int channel){
    switch (channel){
        case 1:
            trig1.update();
            break;
        case 2:
            trig2.update();
            break;
        case 3:
            trig3.update();
            break;
        case 4:
            trig4.update();
            break;
        default:
            //we shouldn't get here unless you gave an incorrect value
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
        
            break;
        
    }
}


void Betweener::readCVInput(int channel){
    switch (channel){
        case 1:
            currentCV1 = analogRead(ANALOG1);

            break;
        case 2:
            currentCV2 = analogRead(ANALOG2);

            break;
        case 3:
            currentCV3 = analogRead(ANALOG3);

            break;
        case 4:
            currentCV4 = analogRead(ANALOG4);

            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;

    }
}



void Betweener::readKnob(int channel){
    switch (channel){
        case 1:
            currentKnob1 = analogRead(KNOB1);
            
            break;
        case 2:
            currentKnob2 = analogRead(KNOB2);
            
            break;
        case 3:
            currentKnob3 = analogRead(KNOB3);
            
            break;
        case 4:
            currentKnob4 = analogRead(KNOB4);
            
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;
            
    }
}


int Betweener::readCVInputMIDI(int channel){
    //first update the value stored for the selected channel
    readCVInput(channel);
    int val=-1;
    //grab the value
    switch (channel){
        case 1:
            val=currentCV1;
            
            break;
        case 2:
            val=currentCV2;
            
            break;
        case 3:
            val=currentCV3;
            
            break;
        case 4:
            val=currentCV4;
            
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;
            
    }
    //convert it and return it:
    return CVtoMIDI(val);

    
}

int Betweener::readKnobMIDI(int channel){
    //first update the value stored for the selected channel
    readKnob(channel);
    int val=-1;
    //grab the value
    switch (channel){
        case 1:
            val=currentKnob1;
            
            break;
        case 2:
            val=currentKnob2;
            
            break;
        case 3:
            val=currentKnob3;
            
            break;
        case 4:
            val=currentKnob4;
            
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;
            
    }
    //convert it and return it:
    return knobToMIDI(val);
    
    
}



int Betweener::readKnobCV(int channel){
    //first update the value stored for the selected channel
    readKnob(channel);
    int val=-1;
    //grab the value
    switch (channel){
        case 1:
            val=currentKnob1;
            
            break;
        case 2:
            val=currentKnob2;
            
            break;
        case 3:
            val=currentKnob3;
            
            break;
        case 4:
            val=currentKnob4;
            
            break;
        default:
            DEBUG_PRINTLN("you are trying to read an nonexistent channel!");
            break;
            
    }
    //convert it and return it:
    return knobToCV(val);
    
    
}
    
int Betweener::CVtoMIDI(int val){
    //CV inputs are 10 bit (range 0-1023)
    //midi CC values go from 0 to 127
    int midival = map(val, 0, 1023, 0, 127);
    return midival;
    
}


int Betweener::MIDItoCV(int val){
    //midi CC values go from 0 to 127
    //CV outs are 12 bit (range 0-4095)
    int cvval = map(val, 0, 127, 0, 4095);
    return cvval;
    
}


int Betweener::knobToMIDI(int val){
    //knob inputs are 10 bit (range 0-1023)
    //midi CC values go from 0 to 127
    int midival = map(val, 0, 1023, 0, 127);
    return midival;
    
}

int Betweener::knobToCV(int val){
    //knob inputs are 10 bit (range 0-1023)
    //CV outs are 12 bit (range 0-4095)
    int cvval = map(val,0,1023,0,4095);
    return cvval;
}



void Betweener::MCP4922_write(int cs_pin, byte dac, int value){
    //this function comes from code written by somebody else and we need to insert the credit...
    
    byte low = value & 0xff;
    byte high = (value >> 8) & 0x0f;
    dac = (dac & 1) << 7;
    digitalWrite(cs_pin, LOW);
    SPI.transfer(dac | 0x30 | high);
    SPI.transfer(low);
    digitalWrite(cs_pin, HIGH);
}

#ifdef DODIGIPOTS
void Betweener::AD5241_write(byte address, int value){
    

    Wire.beginTransmission(address); // transmit to device address
    Wire.write(byte(0x00));            // sends instruction byte
    Wire.write(value);             // sends potentiometer value byte
    Wire.endTransmission();     // stop transmitting
}
#endif

void Betweener::writeCVOut(int value, int cvout){
    int cs_pin = -1;
    byte dac = -1;
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
        default:
            DEBUG_PRINTLN("you are trying to write to a nonexistent CV channel!");
            break;
       
    }
    
    //should put in some idiot checks here that the value is reasonable...
    
    MCP4922_write(cs_pin, dac,value);
}

#ifdef DODIGIPOTS
void Betweener::writeDigipotOut(int value, int digipot){
    byte addr = -1;

    switch (digipot){
        case 1:
            addr = DIGIPOTADDR1;
            break;
        case 2:
            addr = DIGIPOTADDR2;
            break;
        case 3:
            addr = DIGIPOTADDR3;
            break;
        case 4:
            addr = DIGIPOTADDR4;
            break;
        default:
            DEBUG_PRINTLN("you are trying to write to a nonexistent digipot channel!");
        break;
        
    }
    
    //should put in some idiot checks here that the value is reasonable...
    
    AD5241_write(addr,value);
}
#endif

