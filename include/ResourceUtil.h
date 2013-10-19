// Resource Utility Header File

#pragma once

#ifndef __RESOURCEUTIL_H
#define __RESOURCEUTIL_H

#include <stdint.h>
#include <mutex>

namespace Timewhale
{
	enum ResourceType {TEXTURE, AUDIO, BINARY, OTHER};

	struct rsc_desc
	{
		std::string  _path;
		std::string  _name;
		std::mutex*	 _load_lock;
		int32_t		 _blub;
		uint32_t	 _offset;
		uint32_t	 _size;
		uint32_t	 _tag;
		uint8_t		 _scope;
		uint32_t	 _references;
		ResourceType _type;
		bool		 _loading;
		bool		 _hot;
		bool		 _dirty;

		rsc_desc()
			: _path(), _name(), _load_lock(new std::mutex()), _blub(-1), 
			  _offset(0), _size(0), _tag(0), _scope(0), _references(0), 
			  _type(OTHER), _loading(false), _hot(false), _dirty(false)
		{}
	};

	struct PackEntry
	{
		std::string Name;
		int32_t XPos;
		int32_t YPos;
		int32_t Width;
		int32_t Height;

		PackEntry(std::string name, std::string x, std::string y, std::string w, std::string h)
			: Name(name), XPos(atoi(x.c_str())), 
			  YPos(atoi(y.c_str())), Width(atoi(w.c_str())), 
			  Height(atoi(h.c_str())) {}
	};

	//typedef unsigned char      uint8_t;
	//typedef unsigned short    uint16_t;
	//typedef unsigned int      uint32_t;
	//typedef unsigned long int uint64_t;

	//typedef			 char		int8_t;
	//typedef			 short	   int16_t;
	//typedef			 int	   int32_t;
	//typedef			 long int  int64_t;

	const unsigned int MAX_SCOPES = 200;

	struct DataPacket
{
	unsigned char* _Data;
	unsigned int _DataLength;

	DataPacket(unsigned char* Data, unsigned int DataLength)
		: _Data(Data), _DataLength(DataLength) {}
	~DataPacket()
	{
		if(_Data)
		{
			free(_Data);
			_Data = nullptr;
		}
	}

	DataPacket(const DataPacket& other)
	{
		*this = other;
	}

	DataPacket& operator= (const DataPacket& other)
	{
		if (&other == this)
			return *this;

		_Data = other._Data;
		_DataLength = other._DataLength;

		return *this;
	}
};
}

#endif