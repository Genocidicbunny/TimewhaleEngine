
#include "SettingsManager.h"
#include "StringUtils.h"
#include "TWLogger.h"
#include <iostream>
#include <fstream>
using namespace std;
using namespace Timewhale;

SettingsManagerPtr SettingsManager::sManager;

SettingsManager::SettingsManager()
	: mFilename(TW_DEFAULT_SETTINGS_FILE)
{}

SettingsManager::SettingsManager(const SettingsManager &that) {
	*this = that;
}

SettingsManager &SettingsManager::operator= (const SettingsManager &that) {
	if (this == &that) return *this;
	mSettings = that.mSettings;
	mFilename = that.mFilename;
	return *this;
}

SettingsManager::~SettingsManager() {}

SettingsManagerPtr const SettingsManager::create() {
    if (!sManager)
		sManager = SettingsManagerPtr(new SettingsManager());
    return sManager;
}

void SettingsManager::shutdown() {
	mSettings.clear();
	sManager.reset();
}

bool SettingsManager::createStandard() {
	return true;
}

bool SettingsManager::parseFile(const vector<string> &strVec) {
    auto itor = strVec.begin();
    auto end = strVec.end();

    for (; itor != end; ++itor) {
        if ((*itor).substr(0, 2) == "//")
            continue; //Its a comment.
            
        vector<string> setting = split(*itor, " ");
        if (setting.size() < 2) {
            //log_sxerror("SettingsManager", "Invalid setting format \"%s\".", itor->c_str());
            continue; //Invalid setting.
        }

        if (setting.size() > 2) {
            //Its a list setting which isn't implemented yet.
            continue;
        }
        else {
            setVar(setting.at(0), setting.at(1));
        }
    }
    
    return true;
}

void SettingsManager::setFilename(const string &filename) {
	mFilename = filename;
}

bool SettingsManager::loadSettings(const string &filename) {
	if (filename == "") return false;

    ifstream fin(filename.c_str());
	if (fin.fail()) {
        log_sxerror("SettingsManager", "Failed to open file %s", filename.c_str());
		return false;
	}

	fin.seekg(0, ios::end);
	int count = (int)fin.tellg();
	char *data = nullptr;
	if (count > 0) {
		fin.seekg(ios::beg);
		data = new char[count+1];
		fin.read(data, count);
		data[count] = '\0';
	}
	fin.close();

    string str(data);
    if (data)
        delete [] data;
    
    return parseFile(split(str, "\n"));
}

bool SettingsManager::saveSettings(const string &filename) {
    if (filename == "") return false;

    ofstream fout(filename.c_str(), ios::trunc);
    if (fout.fail()) {
        log_sxerror("SettingsManager", "Failed to create file %s", filename.c_str());
        return false;
    }

    auto itor = mSettings.begin();
    auto end = mSettings.end();

    for (; itor != end; ++itor) {
        fout << itor->first << " " << (string)(*(itor->second)) << endl;
    }

    fout.close();
	return true;
}

void SettingsManager::addVar(const string &name, IDatorPtr &var) {
    mSettings.insert(make_pair(name, var));
}

void SettingsManager::setVar(const string &name, const string &value) {
    (*mSettings[name]) = const_cast<string&>(value);
}

void SettingsManager::removeVar(const string &name) {

}
