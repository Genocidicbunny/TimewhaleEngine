// File Stream Resource Header File

#pragma once

#ifndef __FILESTREAM_H
#define __FILESTREAM_H

#include "IResource.h"

namespace Timewhale
{
	enum FILE_POS {WHALE_BEGIN = SEEK_SET, WHALE_CURR = SEEK_CUR, WHALE_END = SEEK_END};

	class twFileStream : public twResource
	{
	private:
	public:
		uint32_t _read(void* dest, uint32_t count, uint32_t& pos);
	};

	class FileStream : public IResource
	{
	private:
		twFileStream* _stm;

	public:
		FileStream(const std::string& name);
		FileStream(uint32_t tag);
		FileStream(const FileStream& other);
		FileStream& operator=(const FileStream& other);

		int Seek(int32_t offset, FILE_POS origin);
		uint32_t Read(void* dest, uint32_t count);
		
	};
}

#endif