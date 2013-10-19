/* 
 * Profile Sample Class
 * This class will record time samples for profiling purposes.
 * The the sample starts when it is instantiated and ends when
 * destructed. To use the profiler, include this header file and
 * use the PROFILE(name) macro. The macro will not function if
 * _DEBUG is not defined.
 * 
 * The code for this class is all copy pasta from
 * http://www.gamedev.net/page/resources/_/technical/game-programming/enginuity-part-iii-r1959
 * By Richard Fine
 * He notes he got it from GP Gem 1.
 */
#pragma once
#ifndef __PROFILE_SAMPLE
#define __PROFILE_SAMPLE

#include "IProfilerOutputHandler.h"

#include "SDL\SDL.h"
#include <string>
#include <memory>

#define MAX_PROFILER_SAMPLES 256

#ifdef _DEBUG
#define PROFILE(name) ProfileSample p(name);
#else
#define PROFILE(name)
#endif

namespace Timewhale {

    class ProfileSample {
    protected:
        int mSampleIndex;	//This is the index of the sample in the samples array.
        int mParentIndex;	//This is the index of the parent sample in the samples array.

		/* getTime
		 * This Method gets time for profiling. Time is using SDL ticks
		 * which is then being converted to seconds. */
        inline float getTime() { 
			return ((float)SDL_GetTicks())/1000.0f; 
		}

		/* _profileSample
		 * This struct holds all the data for sample. */
        static struct _profileSample {
            bool isValid;			//True if the sample has good data.
			bool isOpen;			//True if the sample is currently recording.
            unsigned int callCount;	//The number of times that the sample has be recorded
            std::string name;		//Name identifier of the sample. Samples are unique by name.

			/* These are the time values for the sample.
			 * startTime is the time when the sample started.
			 * totalTime is the total execution time of the sample.
			 * childTime is the total time taken by child samples (samples within samples). */
            float startTime, totalTime, childTime;

			/* These are the relative percentages of cpu time taken by the samples.
			 * averagePc is the average percent of time taken.
			 * minPc is the minimum percentage recorded.
			 * maxPc is the maximum percentage recorded.
			 * min and max may not be all that useful. */
            float averagePc, minPc, maxPc;
            
			int parentCount;			//This is the number of parents the sample has		
            unsigned long dataCount;	//This is the number of times the sample has been recorded.

			/* Constructor */
            _profileSample() 
                : isValid(false), dataCount(0), 
                averagePc(-1.0f), minPc(-1.0f), maxPc(-1.0f) 
            {}
        } samples[MAX_PROFILER_SAMPLES];	// Static array of profile samples. I think regular array is for speed

        static int sLastOpenedSample;		// This is the index to the last created sample.
        static int sOpenSampleCount;		// This is the number of active samples.
        static int sNumSamples;				// This is the number of samples created.
        static float sRootBegin, sRootEnd;  // These are the begin and end times of the program. Percentages are based off these.

    public:
		/* ouput
		 * This method outputs the result of the profiler. Output requires a set
		 * Output handler that inherits the IProfilerOutputHandlerPtr. */
        static void output();

		/* resetSample
		 * This method resets the data of the named sample. Nothing happens
		 * if the sample doesn't exist.
		 *\param	string		The name of the sample.	*/
        static void resetSample(const std::string sampleName);

		/* resetAll
		 * This method resets the data of all samples in the profiler. */
        static void resetAll();

		/* This is a shared ptr to the output handler. 
		 * This needs to be set before the profiler can
		 * Output data.	*/
        static IProfilerOutputHandlerPtr sOutputHandler;	

		/* Constructor 
		 * If the name of the sample currently exists and is open then
		 * an error will be thrown. If it is not open it will reopen the
		 * sample. Otherwise it will just create the new sample like normal.
		 *\param	string		Name of the sample.	*/
        ProfileSample(const std::string &sampleName);
        ~ProfileSample();
    };

}

#endif
