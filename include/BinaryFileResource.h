// Binary File Resource Header

#pragma once

#ifndef __BINARYFILE_H
#define __BINARYFILE_H

#include "IResource.h"

namespace Timewhale
{
	//enum FILE_POS {WHALE_BEGIN = SEEK_SET, WHALE_CURR = SEEK_CUR, WHALE_END = SEEK_END};

	class twFile : public twResource
	{
		friend class WhaleFile;
	private:
		uint32_t _file_size;
		uint8_t* _data;

	public:
		twFile(uint8_t* data, uint32_t len);
		inline uint32_t Size() {return _file_size;}
		uint32_t _read(void* dest, uint32_t count, uint32_t& pos);

		uint32_t _archive_pos;
	};

	class WhaleFile : public IResource
	{
	private:
		twFile* _file;
		uint32_t _curr_position;
		
	public:
		WhaleFile(const std::string& name);
		WhaleFile(uint32_t tag);
		WhaleFile(const WhaleFile& other);
		WhaleFile& operator=(const WhaleFile& other);

		int Seek(int32_t offset, FILE_POS origin);
		uint32_t Read(void* dest, uint32_t count);

		uint32_t Tell();
		uint32_t Size();
		uint8_t* GetMem();
	};
}

#endif