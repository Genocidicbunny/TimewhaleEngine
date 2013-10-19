
#include "LogMessage.h"
#include <string>
#include <stdio.h>
#include <cstring>
#include <string>
#include <thread>
#include "TWLogger.h"

#define PST (-8)

const char* LogMessage::logLevelsAsStrings[] = {
    makeString( INFO ),
    makeString( DEBUG ),
    makeString( WARN ),
    makeString( ERROR ),
    makeString( EXCEPTION ),
    makeString( FATAL )
};

LogMessage::LogMessage(LogMessage::LogLevel level, std::string message, const char* fileName,
                       int lineNumber){
    time_t now = time(NULL);
    GetLocalTime(&mTimeStamp);
    mLevel = level;
    mMessage = message;
    mFileName = fileName;
    mLineNumber = lineNumber;
}
LogMessage::~LogMessage() {
	//if(mFileName) delete mFileName;
}

std::string LogMessage::logLevelToString(LogMessage::LogLevel level){
    return std::string(logLevelsAsStrings[level*sizeof(char)]);
}


//todo[lt] make this not windows-only
//returns number of characters written, or a negative number if an error occured
int LogMessage::write(FILE* logFile){
    if(logFile == NULL){
        //todo throw error or something
		return 0;
    }
   return fprintf(logFile, "| %02u:%02u:%02u:%03u | %s | %s | %s:%d\n", mTimeStamp.wHour, mTimeStamp.wMinute,
       mTimeStamp.wSecond, mTimeStamp.wMilliseconds, logLevelToString(mLevel).c_str(),
       mMessage.c_str(), mFileName, mLineNumber);
   //return written;
}
int LogMessage::writeInfo(FILE* logFile) {
	if(logFile == NULL) {
		//error
		return 0;
	}
	return fprintf(logFile, "| %02u:%02u:%02u:%03u | %u | %s | %s\n", mTimeStamp.wHour, mTimeStamp.wMinute,
		mTimeStamp.wSecond, mTimeStamp.wMilliseconds, std::this_thread::get_id().hash(), logLevelToString(mLevel).c_str(),
		mMessage.c_str());
}