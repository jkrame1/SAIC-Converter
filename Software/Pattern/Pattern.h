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
//  I tried dynamic memory allocation for the tables but it got
//  buggy in odd ways.  For now, use the MAXNSAMP and MAXNSEG parameters
//  here to pick reasonable sizes for your tables.
//
//  No effort has been made yet to optimize this for speed or memory.
//  Also, right now it is totally unfinished and only partially tested.
//
//  Revision history:
//  Jan 02 2018 first minimally functional version - KS
//  Jan 14 2018 new constructor, playback disruptions, reverse, stretching, scaling
//
//  To do:
//    -sine functions and ramp curves
//    -callbacks
//    -interpolation
//    ** known bug:  if the processing speed is too fast, next() somehow does not keep up.
//    for the time being, be sure to use some delay
//    ** second known bug:  having problems if there is only one segment.  not sure what that is about.
//    ** and another:  if your sketch delays more than 2 samples-worth, something weird happens
//    fix the ambiguity between what happens with stop() and reset() to be something more logical


#ifndef Pattern_h
#define Pattern_h

#include <math.h>
#include <Arduino.h>


// maximum number of segments you can define
#define MAXNSEG 256
// max table size (really didn't want to have to do this...)
#define MAXNSAMP 1024

// the variable below is a flag determining that if we have to make internal
// adjustments to 'rationalize' the pattern, the delta-t is taken as
// the inflexible/fixed parameter, and instead length and/or n will change.
// if you comment it out, then length is taken as the priority.  
#define DTPRIORITY


#define DEBUG  //leave this line in to have automatic error printouts to Serial monitor; comment out otherwise
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
    Pattern(int n_segments, int millisec_time_step, int* segment_lengths_ms); //segment_time_bounds needs
                                                                            //n_segments elements
  //  ~Pattern();
    
    
    //***************
    // Functions to make the pattern:
    //***************
    void fillSteps(float* step_sequence);  //step_sequence must have n_segments elements
    void fillRamps(float* ramp_endpoints); //ramp_endpoints must have n_segments+1 elements
    void fillFrom(float* list_of_values); //list of values must have enough!
    void fillSegmentFrom(int segment_index, float* list_of_values);  //list of values must have enough!
    
    void assignConstToSegment(int segment_index, float val);
    void assignConstToSegments(int segment_index_start, int segment_index_end, float val);
    void assignRampToSegment(int segment_index, float startval, float endval);
    void assignRampToSegments(int segment_index_start, int segment_index_end, float startval, float
                               endval);
 
    void setRandMinMax(int min, int max); //min and max parameters for random number generator
                                          //defaults to 0 to 127
    //use integer random numbers with min and max defined by the setRandMinMax function to
    //fill specified segments any time they are playing:
    void assignRandToSegment(int segment_index);
    void assignRandToSegments(int segment_index_start, int segment_index_stop);

    

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
    //rescales whole pattern.  NOTE:  does not rescale any random segments.  change those with
    //the setRandMinMax function.
    void rescaleTo(float minval, float maxval);
 
    //this is a placeholder... I think actually that quantizing should be left to the
    //pattern bank code and not put here.
//    void quantizeTo(int nsteps, int min, int max);   //not implemented yet
    
    
    
    
    //***************
    //Functions related to playing/displaying the pattern
    //***************

    float next();  //main function for playback in a loop.
    
    float once();  //main function for once-through playback
    
    void hold();   //continues returning the same value until unHold is called
    void unHold();

    
    void stop();  //stops playback (just sets an internal flag that makes things reset at next next() call)
    
    void reverse(); //as soon as this is called, begins playing pattern backwards
    void forward();  //returns to forward progression
    
    // stretch or compress the time intervals
    void scaleTime(float scalefactor);  //for whole pattern
    void scaleSegmentTime(int segment_index, float scalefactor);  //for just one segment
    //jump around
    void jumpToSeg(int segment_index);
    void jumpTo(int sample_index);
    
    void reset(); //restores original time
    
    bool done(); //returns a flag for whether the pattern has gone through one complete cycle
    
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
    
    
    void init(); //private initialization function.  Called by both constructors.  Sets defaults.
    
    
    //using the convention that an added underscore at the end of each variable name
    //denotes that these are private variables that belong to the class itself
    float length_;  //length of pattern in seconds
    float lengthmsec_;
    int nseg_;  //number of segments
    uint dt_;  //delta-t, a.k.a. the time step size in millisec
    uint odt_; //original delta-t.  Stores a copy in case we modify it
    int nsamp_;
    int npersegment_[MAXNSEG];
    int segstarts_[MAXNSEG];
    int segends_[MAXNSEG];
    int isamp_;  //index used for looping through to play the pattern
    int iseg_; //index used for tracking which segment we are in
    bool forward_;
    bool isPaused_;
    bool doRandom_[MAXNSEG];
    long randMin_;
    long randMax_;
    elapsedMillis ticker_; //timer we use for looping
    elapsedMillis pauseTicker_; //timer that lets us reset to original timer after a hold/pause
    bool isPlaying_;
    float lastVal_;
    float lastTime_;
    float tScale_;
    float segTscale_[MAXNSEG];
    byte whichSeg_[MAXNSAMP]; //which segment is a given sample in; has length nsamp_
    int seglengths_[MAXNSEG];
    
    
    

    bool done_; //flag for tracking if a complete cycle of the pattern has been completed, for once()
    
    bool equal_; //flag for whether it's equal-time segments or not
    
//    int i_;  //this is the index of the time stamp relative to start of pattern
//    int j_;  //this is the index of the time stamp relative to start of segment
//    int segment_;
    float table_[MAXNSAMP];
    
};




#endif /* Pattern_h */
