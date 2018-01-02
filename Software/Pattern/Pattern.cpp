//
//  Pattern.cpp
//  
//
//  Created by kathryn schaffer on 12/29/17.
//
//

#include "Pattern.h"

Pattern::Pattern(float length_in_sec, int n_segments, int millisec_time_step){
 
    //note to self... code hangs if you ask for more segments than will fit given
    //time and dt values.  Need a check and correction for that condition
    
    length_ = length_in_sec;
    lengthmsec_ = length_ * 1000.;
    nseg_ = n_segments;
    dt_ = millisec_time_step;

    
    //test and rationalize the inputs - that is, ensure that length is an
    //integer multiple of the dt value. If it isn't, we make different decisions if
    //the DTPRIORITY flag is set or not.

    float remainder = fmod(lengthmsec_, dt_); //fmod is defined in math.h; it calculates remainder
    nsamp_ = (int) (lengthmsec_ / dt_); //calculates integer number of dt samples that will fit

    //if the number of samples requested exceeds the maximum table length, we have a
    //problem.  Issue an error message and truncate the pattern so that it will fit.
    
    if (nsamp_ > TABLELENGTH){
        DEBUG_PRINT("Requested pattern exceeds the available array length for the table.");
        DEBUG_PRINT("Modifying the pattern to fit.");
        
        nsamp_ = TABLELENGTH;
#ifdef DTPRIORITY

        lengthmsec_ = nsamp_ * dt_;
        length_ = lengthmsec_ * 1000.;
#else
        dt_ = lengthmsec_ / nsamp_;
#endif
        
    }  else {
        
        //Even if the number of samples fits, we could have a problem that there's not an
        //integer number of time samples in the requested length.  We will correct this in
        //different ways depending on the DTPRIORITY flag
    
#ifdef DTPRIORITY
        //in this case, we are taking the dt value as the most important time unit
        //and rationalizing to that
        //so if the specified time is longer than the nearest integer number of dt samples, we reset it
        if (remainder > 0.){
            lengthmsec_ = nsamp_ * dt_;
            length_ = lengthmsec_ / 1000.;
            DEBUG_PRINT("adjusting the total length of your pattern to be an integer number of samples");
        }
#else
        //in this case, we are taking the length of the pattern as the most important time unit
        //and rationalizing to that.  Thus if the dt is off, we reset it.
        if (remainder > 0.){
            dt_ = lengthmsec_ / nsamp_;
            DEBUG_PRINT("adjusting the time step of your pattern to be an integer divisor of the length");
        }
#endif
    }
    
    float timeperseg = lengthmsec_ / nseg_;
    
    //check that number of segments is not too many
    if (nseg_ > MAXNSEG){
        DEBUG_PRINT("you requested more than MAXNSEG segments.  Truncating to MAXNSEG.");
        nseg_ = MAXNSEG;
    }
    //check that the number of segments is not too few (need at least one time sample per segment)
    if (timeperseg < dt_){
        DEBUG_PRINT("you requested more segments than time points.  Making each time point a segment.");
        nseg_ = nsamp_;
    }

    //fill in arrays of start/stop indexes for each segment

    int n = 0; //index of current segment
    float currenttime = 0.;
    bool newseg = true;
    // we loop through all samples and assign start/stop indexes to the segments.
    // the reason for doing it this way is so that the number of segments truly divides
    // the total time, not requiring that a perfectly even number of samples
    // be in each segment
    segstarts_[0] = 0;
    segends_[0] = 0;
    for (int k=0; k<nsamp_; k++){
        currenttime = k*dt_;
//        if (newseg == true){
//           // segstarts_[n] = k;
//            segstarts_[n] = k;
//            segends_[n] = k;
//            newseg = false;
//        }else{
            if (currenttime < ((n+1) * timeperseg)){
                //segment end index gets overridden until
                // we reach the 'else' condition
                segends_[n] = k;
            }else{
                if (n+1 < nseg_){
                    segstarts_[n+1] = k;
                    segends_[n+1] = k;
                }
                newseg = true;
                n = n+1;
            }
//        }
    }
    //final end point:
    segends_[nseg_ - 1] = nsamp_ - 1;
    
    
    //initialize table to all zeros
    for (int k = 0; k < nsamp_; k++){
        table[k]=0.;
    }
  
    
    //initialize index variable to 0
    isamp_ = 0;
    
    //initialize to a non-playing state (we are not tracking time)
    isPlaying_ = false;
    
}

void Pattern::assignConstToSegment(int segment_index, float val){
    
    if (segment_index < 0 or segment_index > nseg_){
        DEBUG_PRINT("you are trying to modify a nonexistent segment.  Ignoring your request.");
    }else{
        int n = segends_[segment_index] - segstarts_[segment_index];
        for (int i = 0; i<n+1; i++){
            table[segstarts_[segment_index] + i] = val;
        }
    }
    
}


void Pattern::assignConstToSegments(int segment_index_start, int segment_index_end, float val){
    for (int i = segment_index_start; i<segment_index_end+1; i++){
        assignConstToSegment(i, val);
    }
}

void Pattern::assignRampToSegment(int segment_index, float startval, float endval){
    
    if (segment_index < 0 or segment_index > nseg_){
        DEBUG_PRINT("you are trying to modify a nonexistent segment.  Ignoring your request.");
                                                                                
    }else{
        int n = segends_[segment_index] - segstarts_[segment_index];
        float stepsize =  (endval - startval)/n;
        int begin_index=segstarts_[segment_index];
        for (int i = 0; i<n+1; i++){
            table[begin_index + i] = startval + i*stepsize;
        }
        
    
    }
    
    
}

void Pattern::assignRampToSegments(int segment_index_start, int segment_index_end, float startval, float endval){
    for (int i = segment_index_start; i<segment_index_end+1; i++){
        assignRampToSegment(i, startval, endval);
    }
}

void Pattern::fillSteps(float* step_sequence){
    for (int i = 0; i<nseg_; i++){
        assignConstToSegment(i, step_sequence[i]);
    }
}

void Pattern::fillRamps(float* ramp_endpoints){
    
    assignRampToSegment(0,ramp_endpoints[0],ramp_endpoints[1]);
    
    for (int i=1; i<nseg_-1; i++){
        assignRampToSegment(i,ramp_endpoints[i],ramp_endpoints[i+1]);
    }
}



void Pattern::rescaleTo(float minval, float maxval){
    float currentmax = getMax();
    float currentmin = getMin();
    float newval;
    for (int i =0; i<nsamp_; i++){
        newval=fMap(table[i],currentmin, currentmax, minval, maxval);
        table[i]=newval;
    }
}



float Pattern::next(){
    int nextsamp;
    if (!isPlaying_){
        isPlaying_ = true;
        ticker_ = 0;
        isamp_ = 0;
    }else{
        nextsamp = isamp_ + 1;
        if (ticker_ > (nextsamp * dt_)){
            if (nextsamp < nsamp_){
                isamp_ = nextsamp;
            }else{
                isamp_ = 0;
                ticker_ = 0;
            }
        }
    }
    return table[isamp_];
}


void Pattern::stop(){
    isPlaying_ = false;
}

void Pattern::printToSerial(){
    
    Serial.println(String("Pattern with length ")+length_+String(" seconds, ")+nseg_+String(" segments, and ")+dt_+String(" milliseconds between time samples."));
    Serial.println(String("This pattern has a total of ")+nsamp_+String(" samples"));
    Serial.println("Here are the segment start/stop indices: ");
    for (int i = 0; i<nseg_; i++){
        Serial.println(String("segment: ")+i+String(" ")+segstarts_[i]+String(" ")+segends_[i]);
    }
    
    
    
 //   int n=0; // current segment index
    for (int k = 0; k<nsamp_; k++){
        
        Serial.println(String("index: ")+k+String(" pattern value: ")+table[k]);
    }
                   
}

float Pattern::getMax(){
    float maxval;
    maxval=table[0];
    for (int i=1; i<nsamp_; i++){
        if (maxval < table[i]){
            maxval = table[i];
        }
    }
    return maxval;
}

float Pattern::getMin(){
    float minval;
    minval=table[0];
    for (int i=1; i<nsamp_; i++){
        if (minval > table[i]){
            minval = table[i];
        }
    }
    return minval;
}

float Pattern::fMap(float val, float oldmin, float oldmax, float newmin, float newmax){
    return (val - oldmin) * ( (newmax - newmin)/(oldmax - oldmin) ) + newmin;
}

