#pragma once
#ifndef __LOG_MESSAGE_H__
#define __LOG_MESSAGE_H__
#define makeString( name ) # name

#include <time.h>
#include <stdio.h>
#include <string>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

class LogMessage {
 public:
  enum LogLevel{
    //due to c++ auto-assigning of values, setting INFO to 0
    //will lead to each level after it having the value set
    //to one above the previous value
    INFO = 0,
    DEBUG, //1
    WARN, //2
    ERROR_TW, //3 named ERROR_TW as it is timewhale error, as ERROR is already defined
    EXCEPTION, //4
    FATAL //5
  };

  LogMessage(LogLevel level, std::string message, const char* fileName, int lineNumber);
  ~LogMessage();

  static std::string logLevelToString(LogLevel level);
  static const char** initLogLevelsAsStrings();
  int write(FILE* logFile);
  int writeInfo(FILE* logFile);

 private:
  SYSTEMTIME mTimeStamp;
  std::string mMessage;
  const char* mFileName;
  int mLineNumber;
  LogLevel mLevel;
  static const char* logLevelsAsStrings[];

};
#endif
