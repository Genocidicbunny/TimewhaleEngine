/*
 * Platform Manager class
 * The platform manager abstracts and handles everything that has to do with
 * the operating system, such as creating the window and dealing with messages.
 * This manager is using SDL.

 * The platform manager is a manager! Which means it is singleton and can be
 * referenced from anywhere. Include this header and get the manager with the
 * get method.
 *
 * Example - 
 * 
 * #include "PlatformManager.h" 
 * ...
 * auto platform = PlatformManager::get();
 * ...
*/

#pragma once
#ifndef __PLATFORM_MANAGER
#define __PLATFORM_MANAGER

#include "TimewhaleApi.h"
#include "SDL\SDL.h"
#include "SDL\SDL_syswm.h"
#include "Engine.h"
#include <memory>
#include <string>

namespace Timewhale {

    class PlatformManager {
        SDL_Surface* mSDLSurface;		//This is the SDL surface
        SDL_Event mEvent;				//This is an event that stores the current
										//  message the manager is handling.
        bool mQuit;						//This is true if a quit message was recieved.
		bool mFullScreen;				//This is true if we are in fullscreen mode.
		bool mConsole;					//This is true if a console window is open (not yet implemented).
        uint16_t mWindowWidth, mWindowHeight;//These are the width and height of the window.
        std::string mWindowName;        //This is the name of the window.

		//This is the singleton object for the manager.
        static std::shared_ptr<PlatformManager> sManager;

		/* Constructors */
        PlatformManager();
        PlatformManager(const PlatformManager &);
        PlatformManager &operator= (const PlatformManager &);

    public:
		/* get
		 * This method returns the pointer to the manager. */
        inline static std::shared_ptr<PlatformManager> const &get() {
            return sManager;
        }

        /* create
         * This method creates the manager object. */
        static std::shared_ptr<PlatformManager> const create();

		/* init
		 * This method initializes the manager. It created the window
		 * and stuff. 
		 *\param	int		The width of the window.
		 *\param	int		The height of the window.  */
        bool init();
        
		/* shutdown
		 * This method shuts down the manager. Closes the window. */
		void shutdown();
        
		/* update
		 * This method updates the manager, and processes the OS messages.
		 * It is important to call this every game tick to respond to the
		 * messages.	*/
		void update();

		/* processMessages
		 * This method handles recieved messages. */
        void processMessages();

		/* setFullscreen
		 * This method sets fullscreen mode for the window. This isn't
		 * implemented yet.
		 *\param	bool	True for fullscreen mode, false for windowed. */
        void setFullscreen(const bool);

		/* isFullscreen
		 * This method returns true if the window is in fullscreen mode. */
        const bool isFullscreen() const;
        
		/* width
		 * This method returns the width of the window.
		 *\return	int		The width of the window. */
        const uint16_t width() const;

		/* height
		 * This method returns the height of the window.
		 *\return	int		The height of the window. */
        const uint16_t height() const;

		/* quitting
		 * This method returns true if a quit message was recieved.
		 *\return	bool	True if a quit message was recived. */
        const bool quitting() const;

        /* loadTextFile
         * Loads a textfile into a string. Will be later removed when
         * proper resource loading is implemented.
         *\param    string      Filename to be loaded.
         *\return   string      Data from the text file in a string.    */
        std::string loadTextFile(const std::string &filename);

		void setWindowTitle(const std::string &text);

		int minimizeWindow();

		int restoreWindow();

        ~PlatformManager();
    };

    typedef std::shared_ptr<PlatformManager> PlatformManagerPtr;
}

#endif
