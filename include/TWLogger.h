#pragma once
#ifndef __LOGGER_H__
#define __LOGGER_H__
#include "LogMessage.h"
#include <unordered_map>
#include <string>

#define LOG_TW_INFO
//#define LOG_TW_DEBUG
//#define LOG_TW_WARN
#define LOG_TW_ERROR

#ifdef LOG_TW_INFO
#define log_xinfo(format, ...) \
    do { \
    TWLogger::getLoggerForTag(getLogTag())->xinfo(__FILE__, __LINE__, format, ##__VA_ARGS__); \
    }while(0)
#define log_sxinfo(tag, format, ...) \
	do { \
	TWLogger::getLoggerForTag(tag)->xinfo(__FILE__,__LINE__,format, ##__VA_ARGS__); \
	}while(0)
#else
    #define log_xinfo(format, ...) \
do { (void)(sizeof(format)); } while(0)
#define log_sxinfo(tag, format, ...) 
#endif

#ifdef LOG_TW_DEBUG
#define log_xdebug(format, ...) \
    do { \
    TWLogger::getLoggerForTag(getLogTag())->xdebug(__FILE__, __LINE__, format, ##__VA_ARGS__); \
    }while(0)
#define log_sxdebug(tag, format, ...) \
	do { \
	TWLogger::getLoggerForTag(tag)->xdebug(__FILE__,__LINE__, format, ##__VA_ARGS__); \
	}while(0)
#else
    #define log_xdebug(format, ...) \
do { (void)(sizeof(format)); } while(0)
#define log_sxdebug(tag, format, ...)
#endif

#ifdef LOG_TW_WARN
#define log_xwarn(format, ...) \
    do { \
    TWLogger::getLoggerForTag(getLogTag())->xwarn(__FILE__, __LINE__, format, ##__VA_ARGS__); \
    }while(0)
#define log_sxwarn(tag, format, ...) \
	do { \
	TWLogger::getLoggerForTag(tag)->xwarn(__FILE__,__LINE__, format, ##__VA_ARGS__); \
	}while(0)
#else
    #define log_xwarn(format, ...) \
do { (void)(sizeof(format)); } while(0)
#define log_sxwarn(tag, format, ...)
#endif

#ifdef LOG_TW_ERROR
#define log_xerror(format, ...) \
    do { \
    TWLogger::getLoggerForTag(getLogTag())->xerror(__FILE__, __LINE__, format, ##__VA_ARGS__); \
    }while(0)
#define log_sxerror(tag, format, ...) \
	do { \
	TWLogger::getLoggerForTag(tag)->xerror(__FILE__,__LINE__, format, ##__VA_ARGS__); \
	} while(0)
#else
    #define log_xerror(format, ...) \
do { (void)(sizeof(format)); } while(0)
#define log_sxerror(tag, format, ...)
#endif

class TWLogger {
 public:
   //must use std::string as the key as using a cstring version is unreliable.
  //static std::unordered_map<std::string, const char* > sClassNameToFile;
   //must use std::string as the key as using a cstring version is unreliable.

  static std::string DEFAULT_LOG_FOLDER;
  
  static TWLogger* getLoggerForTag(const std::string& tag);

  static int registerFilePathForTag(const std::string& tag, const char* filePath);

  static const std::string& getFilePathByTag(const std::string& tag);

  static int updateTagToFilePathMapping(const std::string& tag, const char* file);

  static int loadTagMappingsFromFile(const std::string& filename);

  static std::string localPath;
  //todo[lt] - pushed until needed

  static void closeAllLoggers();

  static void freeAll();

  //void info(std::string message);
  void xinfo(const char* fileName, int lineNumber,
                  const char* formatedCString, ...);

  //void debug(std::string message);
  void xdebug(const char* fileName, int lineNumber,
                  const char* formatedCString, ...);

  //void warn(std::string message);
  void xwarn(const char* fileName, int lineNumber,
                  const char* formatedCString, ...);

  //void error(std::string message);
  void xerror(const char* fileName, int lineNumber,
                  const char* formatedCString, ...);

  std::string vaFormat(const char* formatedString, va_list args);

  //static std::vector<char> platStringToCharVector(Platform::String^ pstring);

  //static std::string platStringToStdString(Platform::String^ pstring);

  //static const char* TWLogger::platStringToCharBuffer(Platform::String^ pstring);

  //Windows::Storage::StorageFolder^ TWLogger::createOrOpenDirection(Windows::Storage::StorageFolder^ cwd, Platform::String^ subDirName);

 protected:
  static std::unordered_map<std::string, TWLogger* > sLazyTagToLogger;

  static std::unordered_map<std::string, FILE* > sFileNameToPointer;

  static std::unordered_map<std::string, std::string> sLazyTagToPath;

  std::string mLogFile;

  //keep a cstring version of class name so we don't have the
  //(really low) amount of time to convert from std::string to c_str()
  //when using fprintf
  const char* mCTag;

  const std::string mClassName;

  //any log messages below this level will be ignored.
  LogMessage::LogLevel mHighestLevelToLog;

  static TWLogger* lazyGetByTag(const std::string& name);

  static FILE* getFileHandleByPath(const std::string& relativeFilePath);

  static FILE* getFileHandle(const std::string& filename);


private:
  FILE* mLogHandle;

  FILE* getLogHandle();

  //private so the only way to create a TWLogger is via getLoggerForClass
  TWLogger(const std::string& tag);

  int setLogHandle(FILE* logHandle);

};


/*
todo[lt] implement this better than just a rehash of error
#ifdef LOG_EXCEPTION
#define log_xexception(format, ...) \
    do { \
    TWLogger::getLoggerForEngineObject(getLogTag())->xexception(__FILE__, __LINE__, format, ##__VA_ARGS__); \
    }while(0)
#else
    #define log_xexception(format, ...) \
do { (void)(sizeof(format)); } while(0)
#endif
*/
/*
#ifdef LOG_FATAL
#define log_xfatal(format, ...) \
    do { \
    TWLogger::getLoggerForEngineObject(getLogTag())->xfatal(__FILE__, __LINE__, format, ##__VA_ARGS__); \
    }while(0)
#else
    #define log_xfatal(format, ...) \
do { (void)(sizeof(format)); } while(0)
#endif
*/

  /*
  void exception(std::exception);
  void xexception(std::exception, const char* additionalInfo, ...);
  */
  /*
  void fatal(std::string message);
  void xfatal(const char* fileName, int lineNumber,
                  const char* formatedCString, ...);
                  */

#endif