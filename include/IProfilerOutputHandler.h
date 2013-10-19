/*
 * Profiler Output Handler interface
 * This interface defines how a class can interact with the profiler
 * to record or display profiling data. This is realized in ProfileLogHandler
 * which uses the logger to write profiling data to file. Ideally in the future
 * we will have a togglable UI element that will display the profiling results
 * in real time.
 *
 * The code for this class is all copy pasta from
 * http://www.gamedev.net/page/resources/_/technical/game-programming/enginuity-part-iii-r1959
 * By Richard Fine
 */
#pragma once
#ifndef __IPROFILER_OUTPUT_HANDLER
#define __IPROFILER_OUTPUT_HANDLER

#include <string>
#include <memory>

namespace Timewhale {

    class IProfilerOutputHandler {
    public:
		/* BeingOutput
		 * This method should ready the handler for recieving sample data. */
        virtual void BeginOutput() = 0;

		/* EndOutput
		 * This method should signal the handler that it has recieved all
		 * the sample data and should do whatever it is going to do with it. */
        virtual void EndOutput() = 0;

		/* Sample
		 * This method is used to submit sample data to the output handler.
		 *\param	float	The minimum percentage.
		 *\param	float	The average percentage.
		 *\param	float	The maximum percentage.
		 *\param	int		The number of times the sample was recorded.
		 *\param	string	The name of the sample.
		 *\param	int		The number of parents (used to indent child samples).	*/
        virtual void Sample(
            float min, float avg, float max, 
            int callCount,
            std::string name, 
            int parentCount) = 0;
    };

    typedef std::shared_ptr<IProfilerOutputHandler> IProfilerOutputHandlerPtr;
}

#endif
