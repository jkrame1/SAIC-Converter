//
//  Pattern.h
//  
//  This class both creates and stores a table of numbers representing
//  some mathematical pattern intended to be used musically as an
//  envelope or control sequence.
//
//  The key parameters defining the structure of the pattern
//  are the time duration (in seconds), the number of
//  "segments" into which that time is divided for separately defining
//  piece-wise components of the functional shape, and the
//  timing resolution within those segments (e.g. the time step size).
//
//  there are no restrictions on the range of values used in the pattern.
//  Internally, it is stored as an array of floats.  Functions exist to
//  rescale to a given range (e.g. -1 to 1 or 0 to 127).
//
//  Current implementation follows the common advice that dynamic memory
//  allocation is best avoided for embedded code.  Teensy should be fine
//  using dynamic memory allocation, but we'll play it safe by just
//  pre-allocating the data arrays.  This will set a hard maximum on the
//  number of points available in the pattern, controlled at compile time.
//
//
//  No effort has been made yet to optimize this for speed or memory.
//  Also, right now it is totally unfinished and only partially tested.
//
//  Revision history:
//  Jan 02 2018 first minimally functional version - KS



#ifndef Pattern_h
#define Pattern_h

#include <math.h>
#include <Arduino.h>

// this sets the maximum number of entries in the table, which is
// the maximum number of time points with individually computed function
// values.
#define TABLELENGTH 1024
// maximum number of segments you can define
#define MAXNSEG 32

// the variable below is a flag determining that if we have to make internal
// adjustments to 'rationalize' the pattern, the delta-t is taken as
// the inflexible/fixed parameter, and instead length and/or n will change.
// if you comment it out, then length is taken as the priority.  
#define DTPRIORITY

//If you want the code to give you some error messages in the
//serial monitor, leave the line below. Otherwise, comment it out.
#define DEBUG  //leave this line in to have automatic error printouts to Serial monitor
//this little "preprocessor macro" (which is like a function but processed at a different time)
//will be used to print the debug info.  See examples in the .cpp file for how this gets used.
#ifdef DEBUG
#define DEBUG_PRINT(...) Serial.print(__VA_ARGS__)
#define DEBUG_PRINTLN(...) Serial.println(__VA_ARGS__)
#else
#define DEBUG_PRINT(...)
#define DEBUG_PRINTLN(...)
#endif


class Pattern
{

public:
    
    Pattern(float length_in_sec, int n_segments, int millisec_time_step);
    
    //***************
    // Functions to make the pattern:
    //***************
    void fillSteps(float* step_sequence);  //step_sequence must have n_segments elements
    void fillRamps(float* ramp_endpoints); //ramp_endpoints must have n_segments+1 elements
    
    void assignConstToSegment(int segment_index, float val);
    void assignConstToSegments(int segment_index_start, int segment_index_end, float val);
    void assignRampToSegment(int segment_index, float startval, float endval);
    void assignRampToSegments(int segment_index_start, int segment_index_end, float startval, float
                               endval);
    void assignRandomIntsToSegment(int segment_index, int min, int max);
    void assignRandomIntsToSegment(int segment_index_start, int segment_index_stop, int min, int max);

    
    //placeholders for brainstorming some of the other things I might want to add
//    void assignHoldToSegment(int segmnet_index); // hold last value of previous segment
//    void assignHoldToSegments(int segment_index_start, int segment_index_end);
    //void assignExpUpToSegment(int segment_index, ...
    //void assignExpDownToSegment(...
    //

    //callback functions for assigning functions to segments
    //the index i is the index of the time step from the start of the pattern;
    //index j is the index of the time step from start of segment.  Functions can be defined
    //with i, j, or i and j as parameters.  Callbacks are provided for each option.
    
    //this is not the actual syntax for setting this up... just a placeholder until
    //I look up the details:
 //   void assignFuncOfIToSegment(int segment_index, (void*)func);
 //   void assignFuncOfJToSegment(int segment_index, (void*)func);
 //   void assignFuncOfIJToSegment(int segment_index, (void*)func);
    
    //****************
    //Functions to modify the pattern
    //****************
    void rescaleTo(float minval, float maxval);
    //internally, the table is always stored as floats.  what quantizeTo does is *round* to
    //a set of steps in the specified range, rounding up or down to the nearest interval.
    //this will give a different behavior than casting to int, which just truncates.
    //also, it lets you do things like quantize to 3 steps within a range from 0 to 127
    //if for some reason you might want to do that
 //   void quantizeTo(int nsteps, int min, int max);   //not implemented yet
    
    
    
    
    //***************
    //Functions related to playing/displaying the pattern
    //***************

    float next();  //main function for playback.  returns new values from table only when
                   //enough time has elapsed; otherwise, returns same current value
//    float nextWait();  //this version takes over the main loop until the next time unit change
    
    
    void stop();  //stops playback (just sets an internal flag that makes things reset at next next() call)
    
    //reassuring printout
    void printToSerial(void);
    
    
    //***************
    //Utility functions
    //****************
    float getMax(void);
    float getMin(void);
    //this is just like arduino's regular 'map' function but for floats:
    float fMap(float val, float oldmin, float oldmax, float newmin, float newmax);
    
    
private:
    //using the convention that an added underscore at the end of each variable name
    //denotes that these are private variables that belong to the class itself
    float length_;  //length of pattern in seconds
    float lengthmsec_;
    int nseg_;  //number of segments
    uint dt_;  //delta-t, a.k.a. the time step size in millisec
    int nsamp_;
    int npersegment_;
    int segstarts_[MAXNSEG];
    int segends_[MAXNSEG];
    int isamp_;  //index used for looping through to play the pattern
    elapsedMillis ticker_; //timer we use for looping
    bool isPlaying_;
    
//    int i_;  //this is the index of the time stamp relative to start of pattern
//    int j_;  //this is the index of the time stamp relative to start of segment
//    int segment_;
    float table[TABLELENGTH];
    
};




#endif /* Pattern_h */
