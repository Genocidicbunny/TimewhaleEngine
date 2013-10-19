
#include "ProfileLogHandler.h"
#include "TWLogger.h"

using namespace std;
using namespace Timewhale;

#define PROFILE_HEADER1 

void ProfileLogHandler::BeginOutput() {
    log_sxinfo("ProfilerLog", "  Min   :   Avg   :   Max   :   #   :   Profile Name  ");
    log_sxinfo("ProfilerLog", "------------------------------------------------------");
}

void ProfileLogHandler::EndOutput() {
    log_sxinfo("ProfilerLog", "------------------------------------------------------");
}

void ProfileLogHandler::Sample(
    float min, float avg, float max,
    int callCount,
    std::string name,
    int parentCount) 
{
    char namebuf[256], indentedName[256];
    strcpy_s(indentedName, name.c_str());
    for( int indent=0; indent<parentCount; ++indent )
    {
        sprintf_s(namebuf, " %s", indentedName);
        strcpy_s(indentedName, namebuf);
    }

    log_sxinfo("ProfilerLog", "  %3.1f :  %3.1f  :  %3.1f  :  %3d  :  %s", 
        min, avg, max, callCount, indentedName);
}