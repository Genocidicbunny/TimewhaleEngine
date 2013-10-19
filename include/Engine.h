/*
 * Engine class
 * The engine class will maintain the life of the game state and supporting systems.
 * The engine will initalize the renderer, audio systems and any other created modules.
 *
 * The engine is a singleton, but should not be called from anywhere in gamedev lands.
 */
#pragma once
#ifndef __TIMEWHALE_ENGINE
#define __TIMEWHALE_ENGINE

#include <vector>
#include <memory>

#include "ISystem.h"
#include "AudioSystem.h"
#include "RenderSystem.h"
#include "ResourceSystem.h"
#include "NetworkSystem.h"
#ifdef _DEBUG
#include "ECS.h"
#include "Scene.h"
#endif
#ifdef _DEBUG
#define DEBUG_ONLY(x) x;
#else
#define DEBUG_ONLY(x)
#endif


namespace Timewhale {
    class Engine {
		// TOTALLY DEBUG BRO
		friend class ResourceSystem;

		//This is the singleton object of the engine class
        static std::shared_ptr<Engine> sEngine;

        AudioSystemPtr mAudio;			//The Audio System!!
		RenderSystemPtr mRendering;	    //The Rendering System!!
		ResourceSystem mResources;      //The Resource System!! 
		NetworkSystemPtr mNetwork;
        bool mPaused;				//True if the engine is paused.

		/* Constructors */
        Engine();
        Engine(const Engine &);
        Engine &operator= (const Engine &);
    public:
		/* get
		 * This method returns a pointer to the engine singleton
		 *\return	EnginePtr		The Engine.	*/
        inline static std::shared_ptr<Engine> const &get() {
            return sEngine;
        }

        /* create
         * This method creates the engine object.   */
        static std::shared_ptr<Engine> const create();

		/* init
		 * This method initializes the engine. 
		 *\return	bool	True if init is successful. */
        bool init(vector<string>);

		/* run
		 * This method prepares the engine to be run or unpauses the engine. */
        void run();

		/* update
		 * This method will go through one tick of the game loop updating
		 * all the systems. */
        void update();

		/* pause
		 * This method pauses the engine. */
        void pause();

		/* shutdown
		 * This method shuts down the engine and all systems. */
        void shutdown();

		/* resize
		 * Temporary resize function, so we can resize the rendering window. */
		void resize(int windowWidth, int windowHeight);

        ~Engine();
    };

    typedef std::shared_ptr<Engine> EnginePtr;
}

#endif
