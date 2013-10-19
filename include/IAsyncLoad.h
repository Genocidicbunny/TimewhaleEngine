// Async Load Header File

#pragma once

#ifndef __IASYNCLOAD_H
#define __IASYNCLOAD_H

#include <string>
#include <thread>
#include "IResource.h"

namespace Timewhale
{
	class IAsyncLoad
	{
	private:
	protected:
		std::thread _task;
		int RunTask(const std::string& name, ResourceType type, void(*fucn)(const std::string&, uint32_t tag));
		//int RunTask(uint32_t scope, ResourceType type, void(*fucn)(const std::string&, uint32_t tag));
	public:
		IAsyncLoad(const std::string& name, ResourceType type, void(*fucn)(const std::string&, uint32_t tag)); // Creates an async task
		IAsyncLoad(uint32_t scope, ResourceType type, void(*fucn)(const std::string&, uint32_t tag));
		~IAsyncLoad();
	};
};

#endif