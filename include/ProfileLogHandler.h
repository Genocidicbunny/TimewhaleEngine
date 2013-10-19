/*
 * Profile Log Handler class
 * This class is a simple implementation of the IProfilerOutputHandler interface.
 * This class can be used to print output to file using the logger.
 *
 * The code for this class is all copy pasta from
 * http://www.gamedev.net/page/resources/_/technical/game-programming/enginuity-part-iii-r1959
 * By Richard Fine
*/
#pragma once
#ifndef __PROFILE_LOG_HANDLER
#define __PROFILE_LOG_HANDLER

#include "IProfilerOutputHandler.h"

namespace Timewhale {

    class ProfileLogHandler : public IProfilerOutputHandler {
    public:
		/* BeginOutput
		 * This method needs to be called before sending samples to the handler.
		 * It prints the prints the profiling headers. Probably needs some work
		 * to make look prettier */
        void BeginOutput();

		/* EndOutput
		 * This method needs to be called after samples have been recorded.
		 * It basically just prints a line of dashes. */
        void EndOutput();

		/* Sample
		 * This method is used to submit sample data to the output handler.
		 *\param	float	The minimum percentage.
		 *\param	float	The average percentage.
		 *\param	float	The maximum percentage.
		 *\param	int		The number of times the sample was recorded.
		 *\param	string	The name of the sample.
		 *\param	int		The number of parents (used to indent child samples).	*/
        void Sample(
            float min, float avg, float max,
            int callCount,
            std::string name,
            int parentCount);
    };

    typedef std::shared_ptr<ProfileLogHandler> ProfileLogHandlerPtr;

}

#endif
