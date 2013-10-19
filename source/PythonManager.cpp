
#include "PythonManager.h"
#include "ProfileSample.h"
#include "TWLogger.h"


#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;
using namespace Timewhale;

PythonManagerPtr PythonManager::sManager = nullptr;

PythonManagerPtr const PythonManager::create() {
    if (!sManager) {
        sManager = PythonManagerPtr(new PythonManager());

    }
    return sManager;
}

PythonManager::PythonManager() {}

bool PythonManager::init() 
{
    return true;
}

PythonManager::~PythonManager() {}

void PythonManager::update() {}

void PythonManager::shutdown() {}

PythonManager::PythonManager(const PythonManager &that) 
{
    *this = that;
}

PythonManager &PythonManager::operator= (const PythonManager &that) {
    if (this == &that) return *this;

    return *this;
}

bool PythonManager::callScript(char* scriptName, char* input)
{
    stringstream ss;
    string data;
    char finalCall[200];
    char* pCmdCall = "C:\\Python33\\";

    strcpy(finalCall, "python");
    strcat(finalCall, " ");
    strcat(finalCall, pCmdCall);
    strcat(finalCall, scriptName);
    strcat(finalCall, " ");
    strcat(finalCall, input);

    FILE * f = _popen( "python C:\\Python33\\testUrl.py admin", "r" );
    if ( f == 0 ) {
        log_sxerror("PythonManager", "Could not execute\n" );
        return false;
    }
    const int BUFSIZE = 1000;
    char buf[ BUFSIZE ];
    while( fgets( buf, BUFSIZE,  f ) ) {
        ss << buf;
        ss >> data;
        if(data.at(0) == '0')
            mData.push_back("User name or password incorrect");
        else 
            mData.push_back(data);
        
    }
    _pclose( f );

    return true;
}

std::vector<string> PythonManager::getData()
{
    return mData;
}