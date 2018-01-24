//
//  Pattern.cpp
//

#include "Pattern.h"

Pattern::Pattern(float length_in_sec, int n_segments, int millisec_time_step){
    //This is the constructor function that builds the empty pattern.
    //if user inputs are nonsensical, it corrects them.
    //Fills pattern with zeros to start, and parses out segment start/stop times
    
    length_ = (float)length_in_sec;
 
    lengthmsec_ = length_ * 1000.;
    nseg_ = n_segments;
    dt_ = millisec_time_step;

    //test and rationalize the inputs.  This may result in modifying what the user requested.
    
    float remainder = fmod(lengthmsec_, dt_); //fmod is defined in math.h; it calculates remainder
    nsamp_ = (int) (lengthmsec_ / dt_); //calculates integer number of dt samples that will fit

#ifdef DTPRIORITY
        //in this case, we are taking the dt value as the most important time unit
        //and rationalizing to that
        //so if the specified time is longer than the nearest integer number of dt samples, we reset it
    if (remainder > 0.){
        lengthmsec_ = nsamp_ * dt_;
        length_ = lengthmsec_ / 1000.;
        //    DEBUG_PRINT("adjusting the total length of your pattern to be an integer number of samples");
    }
#else
        //in this case, we are taking the length of the pattern as the most important time unit
        //and rationalizing to that.  Thus if the dt is off, we reset it.
    if (remainder > 0.){
        dt_ = lengthmsec_ / nsamp_;
       //     DEBUG_PRINT("adjusting the time step of your pattern to be an integer divisor of the length");
    }
#endif

  //   equal_ = true;

    
    float timeperseg = lengthmsec_ / nseg_;
    
    
    // we loop through all samples and assign start/stop indexes to the segments.
    // the reason for doing it this way is so that the number of segments truly divides
    // the total time, not requiring that a perfectly even number of samples
    // be in each segment
    int n = 0; //index of current segment
    float currenttime = 0.;
    
    //initialize the first segment
    segstarts_[0] = 0;
    segends_[0] = 0;
    whichSeg_[0]=0;
    for (int k=0; k<nsamp_; k++){
        currenttime = k*dt_;
        if (currenttime < ((n+1) * timeperseg)){
            //segment end index gets overridden until
            // we reach the 'else' condition
            segends_[n] = k;
            //  whichseg_[k]=n;
        }else{
            
            if (n+1 < nseg_){
                segstarts_[n+1] = k;
                segends_[n+1] = k;
            }
            
            n = n+1;
            
        }
        whichSeg_[k]=n;
    }
    //final end point:
    segends_[nseg_ - 1] = nsamp_ - 1;
    
    
    
    //initialize all the variables
    init();
    

}


Pattern::Pattern(int n_segments, int millisec_time_step, int* segment_lengths_ms){
    //this constructor lets you set time windows for the segments so they can be uneven.
    //these are set in millisec
    
    //maybe add a check for stupid stuff (negative inputs etc.)?
    
    // get it all set up
    dt_ = millisec_time_step;
    nseg_ = n_segments;

    int total=0;
    for (int k=0;k<nseg_; k++){
        total+=segment_lengths_ms[k];
    }
    lengthmsec_ = (float)total;
    length_ = lengthmsec_/1000.;
 //   Serial.println(String("length ")+length_);
    nsamp_ = (int)(total/dt_);

    
 //   equal_ = false;

    for (int k=0; k<nseg_; k++){
        seglengths_[k]=segment_lengths_ms[k];
    }
    
    
    
    int i=0; //current seg
    int t=0;  //t in ms
    int accumulated_length = 0;
    segstarts_[0]=0;
    segends_[0] = 0;
    
    for (int k=0; k<nsamp_; k++){
        
        t=k*dt_;
        if (t < accumulated_length+seglengths_[i]){
            
            segends_[i]=k;
            whichSeg_[k]=i;
            
        }else{
            
            accumulated_length = accumulated_length+seglengths_[i];
            
            //increment up and start working with the next segment
            i=i+1;
            segstarts_[i]=k;
            segends_[i]=k;
            whichSeg_[k]=i;
            
        }
        
    }

    
    init();
    
}


//Pattern::~Pattern(){
  //  free(table_);
  //  free(segstarts_);
  //  free(segends_);
  //  free(doRandom_);
  //  free(segTscale_);
  //  free(whichSeg_);
//}


void Pattern::init(){
    
    //all lines below are initializing state and data variables to
    //default settings, many of which will get overridden through function calls
    for (int k = 0; k < nsamp_; k++){
        table_[k]=0.;
        
    }
    
    for (int k=0; k<nseg_; k++){
        doRandom_[k]=false;
    }
    odt_ = dt_;
   
    randMin_ = 0;
    randMax_ = 127;
    isamp_ = 0;
    isPaused_ = false;
    isPlaying_ = false;
    lastVal_ = 0;
    lastTime_ = 0;
    tScale_ = 1.;
    forward_ = true;
    for (int k=0; k<nseg_; k++){
        segTscale_[k] = 1.0;
    }
    done_ = false;



}


void Pattern::setRandMinMax(int min, int max){
    randMin_ = min;
    randMax_ = max;
}


void Pattern::assignConstToSegment(int segment_index, float val){
    
    if (segment_index < 0 or segment_index > nseg_){
        DEBUG_PRINT("you are trying to modify a nonexistent segment.  Ignoring your request.");
    }else{
        int n = segends_[segment_index] - segstarts_[segment_index];
        for (int i = 0; i<n+1; i++){
            table_[segstarts_[segment_index] + i] = val;
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
            table_[begin_index + i] = startval + i*stepsize;
        }
        
    
    }
    
    
}

void Pattern::assignRampToSegments(int segment_index_start, int segment_index_end, float startval, float endval){
    for (int i = segment_index_start; i<segment_index_end+1; i++){
        assignRampToSegment(i, startval, endval);
    }
}




void Pattern::assignRandToSegment(int segment_index){
    if (segment_index > 0 && segment_index <= nseg_){
        doRandom_[segment_index] = true;
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


void Pattern::fillFrom(float* list_of_values){
    //bad things will happen if your list is not long enough, beware!
    //here we just presume it is
    
    for (int i =0; i<nsamp_; i++){
        table_[i]=list_of_values[i];
        
    }
}

void Pattern::fillSegmentFrom(int segment_index, float* list_of_values){
    //we presume the list is long enough
    int k=0;
    for (int i=segstarts_[segment_index]; i<= segends_[segment_index]; i++){
        table_[i]=list_of_values[k];
        k=k+1;
    }
}

void Pattern::rescaleTo(float minval, float maxval){
    float currentmax = getMax();
    float currentmin = getMin();
    float newval;
    for (int i =0; i<nsamp_; i++){
        newval=fMap(table_[i],currentmin, currentmax, minval, maxval);
        table_[i]=newval;
    }
}

bool Pattern::done(){
    //there are conditions under which this will fail... if the
    //amount of time taken for the sketch loop to run is longer thna dt_,
    //we will not catch the short-term flag that shows we just looped
    //around a full cycle.  But otherwise, it should work
    return done_;
}

float Pattern::next(){
  
   
    //this is the main playback function.  Mostly, it pulls values from the
    //internal table and returns them in order with the right timing
    //but it also is where random numbers and holds (pauses) are implemented
    int nextsamp;
    float t;
    float val;
    bool newsamp = false;
  //  float temp;
    if (!isPaused_){
      
        if (!isPlaying_){
            isPlaying_ = true;
            ticker_ = 0;
            isamp_ = 0;
            iseg_ = 0;
            done_ = false;
            t=0;
            lastTime_=0;
            newsamp = true;
          
        }else{
           
            t = dt_ * tScale_ * segTscale_[iseg_];
            if ( ticker_ > lastTime_ +  t){
                lastTime_ = lastTime_+t;
                newsamp = true;
                if (forward_){
                  //  Serial.println("moving forward");
                    nextsamp = isamp_ + 1;
                    if (nextsamp < nsamp_){
                        isamp_ = nextsamp;
                        done_ = false;
                    //    Serial.println("incrementing isamp");
                    } else {
                        isamp_ = 0;
                        done_ = true;
                        ticker_=0;
                        lastTime_=0;
                  
                    }
                }else{
                   // Serial.println("moving in reverse");
                    nextsamp = isamp_ - 1;
                    if (nextsamp > 0){
                        isamp_ = nextsamp;
                        done_=false;
                   //     Serial.println("decrementing isamp");
                    }else{
                        isamp_ = nsamp_-1;
                        done_=true;
                        ticker_=0;
                        lastTime_=0;
                     
                    }
                }
            
                iseg_ = whichSeg_[isamp_];
            }else{
                newsamp = false;
            }

       
       //     float temp=dt_*tScale_*segTscale_[iseg_];
       //    Serial.println(String("ticker: ")+ticker_);
       //     Serial.println(String("lastTime: ")+lastTime_);
       //     Serial.println(String("dt*scales")+temp);
       //     Serial.println(String("sample ")+isamp_+String(" seg ")+iseg_);
      
        }
       
        if (doRandom_[iseg_] == true){
            if (newsamp){
                val = random(randMin_, randMax_);
            }else{
                val = lastVal_;
            }
        }else{
            val = table_[isamp_];
        }
      //  Serial.println(val);
        lastVal_ = val;
  
        return val;
        
    }else{
        
        return lastVal_;
    }
    
}

void Pattern::hold(){
    if (!isPaused_){
        isPaused_ = true;
        pauseTicker_ = 0;
        Serial.println("Pausing");
    }
}

void Pattern::unHold(){
    if (isPaused_){
        isPaused_ = false;
  //      Serial.println(String("ticker: ")+ticker_);

        ticker_ = ticker_ - pauseTicker_;
  //      Serial.println(String("ticker - pauseticker: ")+ticker_);
    }
}
void Pattern::stop(){
    isPlaying_ = false;
}

void Pattern::reverse(){
    forward_ = false;
}

void Pattern::forward(){
    forward_ = true;
}


void Pattern::jumpToSeg(int segment_index){
  //  Serial.println("Jumping!");
    iseg_ = segment_index;
    isamp_ = segstarts_[iseg_];
}

void Pattern::jumpTo(int sample_index){
    isamp_ = sample_index;
    iseg_ = whichSeg_[isamp_];

}

void Pattern::scaleTime(float scalefactor){
  //  Serial.println(String("scaling: ")+scalefactor);
    tScale_ = scalefactor;
}
void Pattern::scaleSegmentTime(int segment_index, float scalefactor){
    segTscale_[segment_index]=scalefactor;
}
void Pattern::reset(){
    tScale_=1.;
    for (int k=0; k<nseg_; k++){
        segTscale_[k]=1.0;
    
    }
}
void Pattern::printToSerial(){
    
    Serial.println(String("Pattern with length ")+length_+String(" seconds, ")+nseg_+String(" segments, and ")+dt_+String(" milliseconds between time samples."));
//    Serial.println(String("length in millisec: ")+lengthmsec_);
    Serial.println(String("This pattern has a total of ")+nsamp_+String(" samples"));
//    Serial.println(npersegment_);
    Serial.println("Here are the segment start/stop indices: ");
    for (int i = 0; i<nseg_; i++){
        Serial.println(String("segment: ")+i+String(" ")+segstarts_[i]+String(" ")+segends_[i]);
    }
    
  
    for (int k = 0; k<nsamp_; k++){
        
        Serial.println(String("index: ")+k+String(" pattern value: ")+table_[k]);
    }
                   
}

float Pattern::getMax(){
    float maxval;
    maxval=table_[0];
    for (int i=1; i<nsamp_; i++){
        if (maxval < table_[i]){
            maxval = table_[i];
        }
    }
    return maxval;
}

float Pattern::getMin(){
    float minval;
    minval=table_[0];
    for (int i=1; i<nsamp_; i++){
        if (minval > table_[i]){
            minval = table_[i];
        }
    }
    return minval;
}

float Pattern::fMap(float val, float oldmin, float oldmax, float newmin, float newmax){
    return (val - oldmin) * ( (newmax - newmin)/(oldmax - oldmin) ) + newmin;
}

