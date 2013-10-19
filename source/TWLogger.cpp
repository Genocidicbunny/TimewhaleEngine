
#include <string>
#include <stdio.h>
#include <unordered_map>
#include <vector>
#include <exception>
#include "TWLogger.h"
#include "share.h"
//#include <windows.storage.h>
//#include <windows.storage.streams.h>
//#include <Windows.ApplicationModel.h>
#include <cwchar>
#include <stdlib.h>
#include <cstdlib>
#include <climits>
#include <ppltasks.h>

//turn printfs on or off for logger debugging
#define DEBUG_ON

#ifdef DEBUG_ON
#define printf(fmt, ...) \
    fprintf(stdout, fmt, ##__VA_ARGS__);
#else
    #define printf(fmt, ...) \
    do { (void)(sizeof(fmt)); } while(0)
#endif

//TODO[lt] make thread safe

//using namespace concurrency;
char currentPath[512];

std::string TWLogger::DEFAULT_LOG_FOLDER = "";

std::unordered_map<std::string, TWLogger* > TWLogger::sLazyTagToLogger;

std::unordered_map<std::string, FILE* > TWLogger::sFileNameToPointer;

std::unordered_map<std::string, std::string > TWLogger::sLazyTagToPath;

std::string TWLogger::localPath =/* TWLogger::platStringToStdString(Windows::Storage::ApplicationData::Current->LocalFolder->Path) +*/ "logs\\";

TWLogger::TWLogger(const std::string& tag) :
    mClassName(tag),
    mCTag(tag.c_str()){
        mLogFile = getFilePathByTag(tag);
        FILE* logHandle = getFileHandleByPath(mLogFile);
      if(logHandle == NULL){
          //todo[lt] handle this case
      } else {
		  SYSTEMTIME curTime;
		  GetLocalTime(&curTime);
		  fprintf(logHandle, "\n\n\tBegin %s log\n\t %02u-%02u-%4u %02u:%02u:%02u:%03u\n\n", tag.c_str(), curTime.wMonth, curTime.wDay, curTime.wYear,
			  curTime.wHour, curTime.wMinute, curTime.wSecond, curTime.wMilliseconds);
	  }
      this->setLogHandle(logHandle);

}

//todo
int TWLogger::loadTagMappingsFromFile(const std::string& filename){
    std::string fullFilePath = localPath + filename;
    FILE* handle;
    char line[256];
    int errcode = fopen_s(&handle, fullFilePath.c_str(), "r");
    if(errcode != 0){
        printf("failed to open file %s with errorcode:%d", fullFilePath.c_str(), errcode);
        //todo[lt] handle error opening file
        return -1;
    }
    while(fgets(line, sizeof(line), handle) != NULL){

    }
	return 0;


}

#pragma optimize("", off)
TWLogger* TWLogger::getLoggerForTag(const std::string& tag){

    TWLogger* specificLogger = TWLogger::lazyGetByTag(tag);
    return specificLogger;
}
#pragma optimize("", on)

const std::string& TWLogger::getFilePathByTag(const std::string& tag){
	typedef std::unordered_map<std::string, std::string>::const_iterator c_itor;
    c_itor keyvalue = sLazyTagToPath.find(tag);
    if(keyvalue == sLazyTagToPath.end()){
        std::pair<c_itor, bool> kv = sLazyTagToPath.emplace(std::make_pair(tag, DEFAULT_LOG_FOLDER+tag));
		//need to handle !kv.second as that means shit went real bad
		keyvalue = kv.first;
	}
    return keyvalue->second;
}


int TWLogger::updateTagToFilePathMapping(const std::string& tag, const char* filePath){
    TWLogger::sLazyTagToPath.insert(std::pair<std::string, const char* >(tag, filePath));
    return 0;
}

//note: deletes all loggers, and closes all files
//maintains Tag->FileName mapping
void TWLogger::closeAllLoggers(){
    for(auto it = sLazyTagToLogger.begin(); it != sLazyTagToLogger.end(); ++it){
        it->second->mLogHandle = NULL;
        delete(it->second);
    }
    sLazyTagToLogger.clear();
    for(auto it = sFileNameToPointer.begin(); it != sFileNameToPointer.end(); ++it){
        fclose(it->second);
        it->second = NULL;
    }
    sFileNameToPointer.clear();
}

void TWLogger::freeAll(){
  /*  TWLogger::closeAllLoggers();
        for(auto it = sLazyTagToPath.begin(); it != sLazyTagToPath.end(); ++it){
        it->second = NULL;
    }*/
    TWLogger::sLazyTagToPath.clear();
}

//non x-level functions will be enabled for the linking between ref classes
//and this logger.

//void TWLogger::info(std::string message){}

void TWLogger::xinfo(const char* fileName, int lineNumber,
                   const char* formatedCString, ...){
	std::string formattedMessage;
	try {
		va_list args;
		va_start(args, formatedCString);
		formattedMessage = vaFormat(formatedCString, args);
		va_end(args);
	} catch (std::exception& e) {
		formattedMessage = "ERROR FORMATTING LOG STRING: "; 
		formattedMessage.append(e.what());
	}
    //todo[lt] optimize the substringing here
    std::string tempCast = std::string(fileName);
    int ind = tempCast.find_last_of('\\');
    std::string localFileName = tempCast.substr(tempCast.find_last_of('\\')+1, tempCast.size());
    LogMessage toLog(LogMessage::LogLevel::INFO, formattedMessage, localFileName.c_str(), lineNumber);
    toLog.writeInfo(this->getLogHandle());
}

//void TWLogger::debug(std::string message){}

void TWLogger::xdebug(const char* fileName, int lineNumber, const char* formatedCString, ...){
	std::string formattedMessage;
	try {
		va_list args;
		va_start(args, formatedCString);
		formattedMessage = vaFormat(formatedCString, args);
		va_end(args);
	}catch (std::exception& e) {
		formattedMessage = "ERROR FORMATTING LOG STRING: ";
		formattedMessage.append(e.what());
	}
    //todo[lt] optimize the substringing here
    std::string tempCast = std::string(fileName);
    int ind = tempCast.find_last_of('\\');
    std::string localFileName = tempCast.substr(tempCast.find_last_of('\\')+1, tempCast.size());
    LogMessage toLog(LogMessage::LogLevel::DEBUG, formattedMessage, localFileName.c_str(), lineNumber);
    toLog.write(this->getLogHandle());
}

//void TWLogger::warn(std::string message){}

void TWLogger::xwarn(const char* fileName, int lineNumber, const char* formatedCString, ...){
	std::string formattedMessage;
	try {
		va_list args;
		va_start(args, formatedCString);
		formattedMessage = vaFormat(formatedCString, args);
		va_end(args);
	} catch (std::exception& e) {
		formattedMessage = "ERROR FORMATTING LOG STRING: ";
		formattedMessage.append(e.what());
	}
    //todo[lt] optimize the substringing here
    std::string tempCast = std::string(fileName);
    int ind = tempCast.find_last_of('\\');
    std::string localFileName = tempCast.substr(tempCast.find_last_of('\\')+1, tempCast.size());
    LogMessage toLog(LogMessage::LogLevel::WARN, formattedMessage, localFileName.c_str(), lineNumber);
    toLog.write(this->getLogHandle());
}

//void TWLogger::error(std::string message){}

void TWLogger::xerror(const char* fileName, int lineNumber, const char* formatedCString, ...){
	std::string formattedMessage;
	try { 
		va_list args;
		va_start(args, formatedCString);
		formattedMessage = vaFormat(formatedCString, args);
		va_end(args);
	} catch (std::exception& e) {
		formattedMessage = "ERROR FORMATTING LOG STRING: ";
		formattedMessage.append(e.what());
	}
		//todo[lt] optimize the substringing here
    std::string tempCast = std::string(fileName);
    int ind = tempCast.find_last_of('\\');
    std::string localFileName = tempCast.substr(tempCast.find_last_of('\\')+1, tempCast.size());
    LogMessage toLog(LogMessage::LogLevel::ERROR_TW, formattedMessage, localFileName.c_str(), lineNumber);
    toLog.write(this->getLogHandle());
}

//turns a format c-string and a variable arguement list into a std::string
//dynamically sizes the buffer so there is no chance of overflow or truncation
std::string TWLogger::vaFormat(const char* formatedCString, va_list args){
	#define _CRT_SECURE_NO_WARNINGS
	//TODO[lt] allow this to be configurable
	size_t size = 1024;
	char buffer[1024];
	std::vector<char> scallingBuffer;
	char* curBufPtr = &buffer[0];

	int additionalSizeNeeded;

	//if we require more space than we have (on linux/macosx vsnprintf will return a number > size
	//whereas on windows vsnprintf will return a negative number on failure
	//the while loop will exit when all the contents of formatedCString and its args fit into the buffer
	while(((additionalSizeNeeded = vsnprintf_s(curBufPtr, (sizeof(char)*size), size, formatedCString, args)) > ((int)size)) ||
		(additionalSizeNeeded < 0)){
			//if we do not currently have enough space, increase size, and try again
			size = (additionalSizeNeeded > 0) ? (additionalSizeNeeded+1) : (size*2);
			scallingBuffer.resize(size);
			curBufPtr = &scallingBuffer[0];
    }

	printf("to return: %s, num characters: %d\n", curBufPtr, additionalSizeNeeded);

	std::string formated(curBufPtr, (size_t) additionalSizeNeeded);
    std::size_t position = 0;
    while((position = formated.find("\r\n", position)) != std::string::npos){
        formated.insert(position++, ' ', 1);
        formated.erase(position, 2);
    }
    return formated;
}

int TWLogger::registerFilePathForTag(const std::string& tag, const char* filePath){
    if(sLazyTagToPath.empty() || (sLazyTagToPath.find(tag) == sLazyTagToPath.end())){
      printf("registering filepath: %s for tag:%s\n", filePath, tag.c_str());
      sLazyTagToPath.insert(std::pair<std::string, const char* >(tag, filePath));        
    } else {
        //hacky way of ignoring multiple calls, will be fixed with xml parsing to register files
        //printf("attempted to register a filepath for tag:%s, when it already has one registered\n", tag);
    }

    return (sLazyTagToPath.find(tag) == sLazyTagToPath.end()) ? -1 : 1;
}

TWLogger* TWLogger::lazyGetByTag(const std::string& tag){
    printf("retrieving logger for tag:%s\n", tag.c_str());
  //if we do not have a TWLogger for the current class
    if(TWLogger::sLazyTagToLogger.empty() || (TWLogger::sLazyTagToLogger.find(tag) 
            == TWLogger::sLazyTagToLogger.end())){
      printf("no logger found with tag:%s, creating one\n", tag.c_str());
      //make one
      TWLogger* logger = new TWLogger(tag);
      //add className -> TWLogger mapping
      TWLogger::sLazyTagToLogger.insert(std::pair<std::string, TWLogger* >(tag, logger));
    }
    //todo[lt] catch exception
    return TWLogger::sLazyTagToLogger.at(tag);
}

//todo[lt] not yet tested
//Windows::Storage::StorageFolder^ TWLogger::createOrOpenDirection(Windows::Storage::StorageFolder^ cwd, Platform::String^ subDirName){
//    Windows::Foundation::IAsyncOperation<Windows::Storage::StorageFolder^>^ asyncFunc = cwd->CreateFolderAsync(subDirName, 
//        Windows::Storage::CreationCollisionOption::OpenIfExists);
//    auto syscall = concurrency::create_task(asyncFunc);
//    Windows::Storage::StorageFolder^ subDir;
//    syscall.then([&subDir] (Windows::Storage::StorageFolder^ newDir){
//        subDir = newDir;
//    });
//
//    Platform::String^ breakTest = subDir->Path;
//    return subDir;
//}

//cannot convert a string larger than 1024 characters
//ONLY WORKS IF pstring only includes ascii characters from 0-128
//if pstring is longer than that, this function will return NULL;
//doesnt work, don't try to use it.
/*const char* TWLogger::platStringToCharBuffer(Platform::String^ pstring){
    std::string sbuffer = platStringToStdString(pstring);
    const char* cbuffer = sbuffer.c_str();
    return cbuffer;
}
*/
//ONLY WORKS ON ASCII VALUES BETWEEN 0 AND 128
//any wchars over 128 will be converted to question marks (?)
//std::vector<char> TWLogger::platStringToCharVector(Platform::String^ pstring){
//    const wchar_t* wcString = pstring->Data();
//    int size = pstring->Length();
//    std::vector<char> vcbuffer(size);
//    char cbuffer[1]; 
//    int charsConverted = 0;
//    for(int i = 0; i < size; i++){
//        wchar_t curWchar = wcString[i];
//        vcbuffer[i] = static_cast<char>(curWchar);
//    }
//    return vcbuffer;
//}

////ONLY WORKS ON ASCII VALUES BETWEEN 0 AND 128
////any wchars over 128 will be converted to question marks (?)
//std::string TWLogger::platStringToStdString(Platform::String^ pstring){
//     
//    const wchar_t* wcString = pstring->Data();
//    int size = pstring->Length();
//    std::vector<char> vcbuffer(size);
//
//    for(int i = 0; i < size; i++){
//        wchar_t curWchar = wcString[i];
//        if(curWchar > CHAR_MAX){
//            curWchar = '?';
//        }
//        vcbuffer[i] = static_cast<char>(curWchar);
//    }
//
//    return std::string(vcbuffer.begin(), vcbuffer.end());
//}

FILE* getFileHandle(std::string filename){

    return NULL;
}

FILE* TWLogger::getFileHandleByPath(const std::string& relativeFilePath){
    printf("getting file handle at location:%s\n", relativeFilePath.c_str());
    if(TWLogger::sFileNameToPointer.empty() || (TWLogger::sFileNameToPointer.find(relativeFilePath)
        == TWLogger::sFileNameToPointer.end())){
            printf("file:%s does not have a handle associated with it. attempting to open\n", relativeFilePath.c_str());
            std::string fullFilePath = localPath + relativeFilePath + ".log";
#pragma warning(disable : 4996) //Disable 'unsafe variable or function' warning for this instance
            FILE* logHandle = fopen(fullFilePath.c_str(), "a+");
			int err;
			_get_errno(&err);
        if(!logHandle){
            printf("failed to open file %s!", fullFilePath.c_str());
            //todo[lt] handle error opening file
            return NULL;
        }
        printf("successfully opened file: %s", relativeFilePath.c_str());
        TWLogger::sFileNameToPointer.insert(std::pair<std::string, FILE*>(relativeFilePath, logHandle));
    }
    //todo[lt] catch exception
    return TWLogger::sFileNameToPointer.at(relativeFilePath);
}

int TWLogger::setLogHandle(FILE* logHandle){
    mLogHandle = logHandle;
    if(mLogHandle != NULL){
        return 1;
    }
    return -1;
}

FILE* TWLogger::getLogHandle(){
	return mLogHandle;
}

/*

currently not implemented as they are not needed initially

void TWLogger::exception(std::exception){}

void TWLogger::xexception(std::exception, const char* additionalInfo, ...){}
*/

/*
void TWLogger::fatal(std::string message){}

void TWLogger::xfatal(const char* fileName, int lineNumber, const char* formatedCString, ...){
	va_list args;
    va_start(args, formatedCString);
    std::string formatedMessage = vaFormat(formatedCString, args);
    va_end(args);
    //todo[lt] optimize the substringing here
    std::string tempCast = std::string(fileName);
    int ind = tempCast.find_last_of('\\');
    std::string localFileName = tempCast.substr(tempCast.find_last_of('\\')+1, tempCast.size());
    LogMessage* toLog = new LogMessage(LogMessage::LogLevel::FATAL, formatedMessage, localFileName.c_str(), lineNumber);
    toLog->write(this->getLogHandle());
}
*/