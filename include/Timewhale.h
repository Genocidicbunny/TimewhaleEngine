/*
 * TimewhaleEngine Class
 * This is the class the game devs will have access to to initialize
 * and control the engine. No managers should be referenced before
 * a call to Initialize. And be certain to call Shutdown before the
 * the program exits.
 */

#pragma once
#ifndef __TIMEWHALE
#define __TIMEWHALE

#include "TimewhaleApi.h"
#include <vector>
#include <string>
//#include <WinSock2.h>
//#include <WS2tcpip.h>

namespace Timewhale {

    class TIMEWHALEAPI TimewhaleEngine {
        static bool running;
    public:
		/* Initialize
		 * This method initializes the engine, managers, and systems.
		 * This must becalled before anything else otherwise nothing will
		 * work and if it does, it will be wrong.
		 *\return	bool		Returns true if the initialization was successful. */
        static bool Initialize(std::vector<std::string> init_packs);

		/* Run
		 * This method starts the main loop. The loop will run until the engine is
		 * paused, or a quit event has been raised. */
        static void Run();

		/* Pause
		 * This method will pause the main loop. To unpause, call run again. */
        static void Pause();

        static void Quit();

		/* Shutdown
		 * This method begins the shutdown procedure, and will clean up all
		 * left over engine stuff. */
        static void Shutdown();
    };
}

#endif
