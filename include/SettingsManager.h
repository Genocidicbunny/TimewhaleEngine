/*
 * SettingsManager
 * The settings manager is responsible for loadings and saving the
 * engine settings files. If a settings file is not present then
 * it will construct one anew.
 *
 * Settings manager is a singleton class and can only be accessed
 * through it's get method.
 * 
 */
#pragma once
#ifndef __SETTINGS_MANAGER
#define __SETTINGS_MANAGER

#include "Dator.h"
#include <unordered_map>
#include <functional>
#include <vector>

#define TW_DEFAULT_SETTINGS_FILE "timewhale_engine.ini"

#define TW_SETTING(type, var, name) SettingsManager::get()->addVar(name, IDatorPtr(new Dator<type>(var)));

namespace Timewhale {

	class SettingsManager {
		typedef std::unordered_map<std::string, IDatorPtr> SettingsMap;

		SettingsMap mSettings;		//This is the map of settings.
		std::string mFilename;		//This is the file name of the settings file.

		/* The singleton Settings Manager. */
		static std::shared_ptr<SettingsManager> sManager;

		/* createStandard
		 * This method generates the standard settings file for the engine.
		 *\return	bool		Returns true if successful.	*/
		bool createStandard();

		/* parseFile
		 * This method parses a vector of strings into a settings map.
		 *\return	bool		Returns true if successful.
								False may imply partial success		*/
		bool parseFile(const std::vector<std::string> &);

		/* Constructors */
		SettingsManager();
		SettingsManager(const SettingsManager &);
		SettingsManager &operator= (const SettingsManager &);
	public:
		/* get
		 * This method returns the SettingsManager singleton.
		 *\return	SettingsManagerPtr		The manager.	*/
		inline static std::shared_ptr<SettingsManager> const get() {   
            return sManager;
        }

        /* create
         * This method created the manager object. */
        static std::shared_ptr<SettingsManager> const create();

		/* shutdown
		 * Shuts down the manager. Make sure to call saveSettings
		 * if you want to say any changes.		*/
		void shutdown();

		/* setFilename
		 * This method sets the filename for the settings file. It
		 * should be set before initialization if you want to load
		 * a different file.
		 *\param	string		The name of the settings file.		*/
		void setFilename(const std::string &);

		/* loadSettings
		 * This method will try to load the settings from a file.
		 * Calling this will completely replace the current settings.
		 * If no filename is given then it will generate the default
		 * settings.
		 *\param	string		The name of the settings file.	
		 *\return	bool		True on success		*/
		bool loadSettings(const std::string &filename = TW_DEFAULT_SETTINGS_FILE);

		/* saveSettings
		 * This method will try to save the settings to disk
		 * using the given filename. If no filename is given
		 * then it will use the default filename.
		 *\param	string		Name of the settings file.
		 *\return	bool		True on success.	*/
        bool saveSettings(const std::string &filename = TW_DEFAULT_SETTINGS_FILE);

		/* addVar
		 * This method adds a variable to the settings. 
		 *\param	string		The name of the variable.
		 *\param	IDatorPtr	A pointer to the dator.	*/
		void addVar(const std::string &name, IDatorPtr &var);

		/* setVar
		 * This method modifies a variable in settings. If the
		 * variable isn't found, nothing happens.
		 *\param	string		The name of the variable.
		 *\param	string		The value to set as a string.	*/
		void setVar(const std::string &name, const std::string &value);

		/* removeVar
		 * This method removes a variable in settings. If the
		 * variable isn't found, nothing happens.
		 *\param	string		The name of the variable.	*/
		void removeVar(const std::string &name);

		~SettingsManager();
	};
	typedef std::shared_ptr<SettingsManager> SettingsManagerPtr;
}

#endif
