///////////////////////////////////////////////////////////////////////////
//  Betweener.h
//  
//  Defines the Betweener class, which sets up the software mappings
//  that enable Teensy to communicate via the hardware setup on the
//  whatever-it's-called device itself.
//
//  The Betweener class also includes functions that allow a
//  relatively automated set of read/write operations so that the
//  device can mediate between CV inputs and outputs, MIDI inputs and
//  outputs, and trigger inputs.
//
//  The class has been written assuming it will be read and modified
//  by users with little coding experience, and is thus obsessively
//  commented with that audience in mind.
//
//  Basic examples for using this code are in a sketch
//  entitled betweener_examples.ino
//
//  REFERENCES AND CREDITS:
//      (need to fill this out)
//
//
//
//  CHANGE LOG:
//  10/23/17 Created by kathryn schaffer (ks)
//
//
//
//
//  TO DO:
//     Error handling
//     Implement optional serial output to be used for debugging/monitoring
//     Make versions of the read functions that read single channels
//     Put in idiot-checks on values sent to be written out
//     Decide on reasonable licensing and liability statement to include here
//
//////////////////////////////////////////////////////////////////////////



//these statements are a standard C++ way to avoid accidentally
//re-loading this .h file multiple times, which can make for weird
//conflicts in parameter and function definitions in certain cases
#ifndef Betweener_h
#define Betweener_h


//Here is where we include all the other Arduino libraries that this
//depends on.  We're using the updated "Bounce2" library because it
//is more compatible with being used in other libraries
#include <SPI.h>
#include <Bounce2.h>


//This is where we define hard-wired pin associations.
//These are "preprocessor directive" statements (with # symbol) instead of
//variable definitions.  We use uppercase names in order to make it
//easy to tell that these are not normal variables when we invoke them in the
//code.  The difference is that #define statements are only implemented when
//the code is compiled, and never when the code is just being run.
//That makes it impossible for any of the code to actively change these
//parameters while it is running, which is a good idea since the
//definitions relate to hard-wired configuration choices.

//SPI is a communication protocol used for the digital-analog-converters (DACs)
//in the Betweener.  These are used to create the CV outputs.
//SPI communication requires two pins for special functions,
//called MOSI and SCK.
#define SPI_MOSI 7 // note this is technically the "alternate" MOSI option for Teensy
#define SPI_SCK 14
//Also for the SPI protocol, these parameters are used to select
//which DAC we are sending messages to
#define DAC_CHIP_SELECT1 1
#define DAC_CHIP_SELECT2 2
//Identify the 4 CV outs with the right chip and DAC
#define CVIN1_CHIP_SELECT 2
#define CVIN1_DAC_CHANNEL 1
#define CVIN2_CHIP_SELECT 1
#define CVIN2_DAC_CHANNEL 0
#define CVIN3_CHIP_SELECT 2
#define CVIN3_DAC_CHANNEL 0
#define CVIN4_CHIP_SELECT 1
#define CVIN4_DAC_CHANNEL 1


//digital pins used for trigger inputs
#define TRIGGER_INPUT1 0
#define TRIGGER_INPUT2 3
#define TRIGGER_INPUT3 5
#define TRIGGER_INPUT4 4
//analog inputs used for the 4 CV inputs
#define ANALOG1 7
#define ANALOG2 6
#define ANALOG3 3
#define ANALOG4 2
//analog inputs used for the 4 potentiometers/knobs
#define KNOB1 12
#define KNOB2 13
#define KNOB3 10
#define KNOB4 11



//////////////////////////////////////////////////////////////////////////////
//Now we define the Betweener class, which will let us make Betweener objects
//in our sketches.  The class organizes a set of functions (called "methods") and
//the set of variables that they use and control.  A special method called the
//"constructor" is the one that initially builds the object and puts in all the
//default variable definitions.  The other methods provide a flexible interface
//for defining how the Betweener maps inputs to outputs and for controlling the order
//of its read/write operations and other operations.

class Betweener
{
    /////////////////////////////////////////////////////////////////////////////
    //The "public" parts of the Betweener class are the ways a user will interact
    //with it.  This includes the "constructor" function (which has the same name
    //as the class itself) that makes and initializes individual Betweener objects
    //in user code. It also includes the functions you will use to make the
    //Betweener do what you want with reading, writing, etc.
    
    public:  //declares the following functions and variables to be available to users
    
    Betweener();  //constructor; does all setup and initialization stuff

    // INPUT FUNCTIONS
    // You have the choice of manually asking the device to read the various
    // types of input one at a time, or asking it to read all of them at once.

    void readTriggers(void);  //reads all triggers
    void readCVInputs(void); //reads analog inputs (CV inputs)
    void readKnobs(void);  //reads potentiometer inputs
    void readUsbMIDI(void);  //reads MIDI via the usbMIDI arduino functionality
    
    void readAllInputs(void); //reads triggers, CV, knobs, and USB MIDI inputs, in that order
    
    
    //BASIC OUTPUT FUNCTIONS
    //These functions assume you are using your sketch to decide directly what
    //output to write.  The functions are mainly useful for just hiding some of the
    //messier logic required by the specific DAC chip, etc.
    void writeCVOut(int value, int cvout); //value is in range 0-4095; cvout selects channel 1 through 4
    
    
    //these are "set" and "get" functions that let the user check and change internal state parameters
    void setBounceMillisec(int millisec){bounce_ms = millisec;};
    int getBounceMillisec(void){return bounce_ms;};
    
    //static means that these functions can be accessed without necessarily having
    //a "Betweener" object available.  Just by including this library/class,
    //they can be accessed via Betweener::receivedNoteOn(...) etc.
    static void MCP4922_write(int cs_pin, byte dac, int value);

    // variables that are used by these functions:
    elapsedMillis msecTickerCVRead;  //ticker for elapsed time since last analog read
    elapsedMillis msecTickerTrigger; //ticker for elapsed time since last trigger
    elapsedMillis msecTickerMIDI; //ticker for elapsed time since last MIDI read

    
    // these are the variables we use to store current analog input readings,
    // updated whenever a read operation is performed
    int currentCV1;
    int currentCV2;
    int currentCV3;
    int currentCV4;

    // these are the variables that store current knob / pot readings,
    // updated whenever a read operation is performed
    int currentKnob1;
    int currentKnob2;
    int currentKnob3;
    int currentKnob4;

    // these are the trigger inputs.  We use the Bounce library, which provides
    // a way to avoid accidental triggers due to fluctuating inputs
    Bounce trig1;
    Bounce trig2;
    Bounce trig3;
    Bounce trig4;
    
    
    
    //the "private" parts of the Betweener class are the interior guts - the ways
    //that the class manages some of the functionality of the device internally.
    //We also have some private variables, which are variables that functions in the
    //class can modify, but a user cannot directly touch... only via a "set" or "get"
    //function.

    
    //Actually I am not sure this is the best place for these parameters... may rethink later
    private:

    int bounce_ms = 5;
    uint analog_read_delta = 20;
  
    
};



#endif /* Betweener_h */
