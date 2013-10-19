/*
 * Python Manager class
 * The python manager abstracts and handles everything that has to do with
 * calling Python scripts and handling the scripts output

 * The python manager is a manager! Which means it is singleton and can be
 * referenced from anywhere. Include this header and get the manager with the
 * get method.
 *
 * Example - 
 * 
 * #include "PythonManager.h" 
 * ...
 * auto python = PythonManager::get();
 * ...
*/

#pragma once
#ifndef __PYTHON_MANAGER
#define __PYTHON_MANAGER

#include "TimewhaleApi.h"
#include "Engine.h"
#include <memory>
#include <string>
#include <vector>

namespace Timewhale {

    class PythonManager {
      
        std::vector<string> mData;


		//This is the singleton object for the manager.
        static std::shared_ptr<PythonManager> sManager;

		/* Constructors */
        PythonManager();
        PythonManager(const PythonManager &);
        PythonManager &operator= (const PythonManager &);

    public:

        std::vector<string> getData();

        bool callScript(char* scriptName, char* input);

        

		/* get
		 * This method returns the pointer to the manager. */
        inline static std::shared_ptr<PythonManager> const &get() {
            return sManager;
        }

        /* create
         * This method creates the manager object. */
        static std::shared_ptr<PythonManager> const create();

		/* init
		 * This method initializes the manager.
		 */
        bool init();
        
		/* shutdown
		 * This method shuts down the manager. */
		void shutdown();
        
		/* update
		 * This method updates the manager
		 */
		void update();

        ~PythonManager();
    };

    typedef std::shared_ptr<PythonManager> PythonManagerPtr;
}

#endif
