/*
 * System Interface
 * This interface describes a system. Currently this isn't
 * being used but a system is any underlaying engine component
 * whose life span is managed by the engine. Generally a system
 * is not directly accessed by the game developers.
 */
#pragma once
#ifndef __ISYSTEM
#define __ISYSTEM

#include <memory>

namespace Timewhale {

    class ISystem {
		/* init
		 * This method should initialize the system and get it ready to update.
		 *\return	bool		True if initialization was successful.	*/
        bool init();

		/* shutdown
		 * This method shuts down the system and cleans up any loose data it may have. */
        void shutdown();

		/* pause
		 * This method will pause the system, and prepare for not recieving update calls. */
        void pause();

		/* update
		 * This method will update the system. */
        void update();
    };

    typedef std::shared_ptr<ISystem> ISystemPtr;

}

#endif
