#pragma once

//Propose renaming to WhaleBrain.h

#ifndef __TIMEWHALE_TWMEMORY_H_
#define __TIMEWHALE_TWMEMORY_H_

//#include "Timewhale\ApiSpec.h"
#ifdef _MSC_VER 
#define ALIGN_OF(T) __alignof(T)
#define ALIGN_AS(NUM) __declspec(align(NUM))
#else
#define ALIGN_OF(T) alignof((T))
#define ALIGN_AS(NUM) alignas((NUM))
#endif

//Because I dont know where else to stick this right now
#include <process.h>
#include <string>
#include <iostream>
#include <sstream>

static size_t GetPID() {
	return (size_t)_getpid();
}
static std::string& GetPIDStr() {
	static std::string pidStr;
	static bool callonce = true;
	if(callonce) {
		callonce = false;
		std::stringstream str;
		str << GetPID();
		pidStr = str.str();
	}
	return pidStr;
}

#include <memory.h>
namespace Timewhale {
	//Taken from:
	//--------------------------------------------------------------------------------------
	// File: AlignedNew.h
	//
	// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF
	// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO
	// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A
	// PARTICULAR PURPOSE.
	//
	// Copyright (c) Microsoft Corporation. All rights reserved.
	//
	// http://go.microsoft.com/fwlink/?LinkId=248929
	//--------------------------------------------------------------------------------------
	template<typename TDerived>
	struct AlignedNew
	{
		// Allocate aligned memory.
		static void* operator new (size_t size)
		{
			const size_t alignment = ALIGN_OF(TDerived);

			static_assert(alignment > 8, "AlignedNew is only useful for types with > 8 byte alignment. Did you forget a _declspec(align) on TDerived?");

			void* ptr = _aligned_malloc(size, alignment);

			if (!ptr)
				throw std::bad_alloc();

			return ptr;
		}


		// Free aligned memory.
		static void operator delete (void* ptr)
		{
			_aligned_free(ptr);
		}


		// Array overloads.
		static void* operator new[] (size_t size)
		{
			return operator new(size);
		}


		static void operator delete[] (void* ptr)
		{
			operator delete(ptr);
		}
	};
	//END AlignedNew.h
}
#endif