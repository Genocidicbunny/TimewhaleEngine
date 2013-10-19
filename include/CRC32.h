#pragma once
#ifndef __TIMEWHALE_CRC32_H_
#define __TIMEWHALE_CRC32_H_
#include <cstdint>


namespace Timewhale {

	class CRC32 {
	private:
		static uint32_t crc32Table[256];
		static void _crc32(const uint8_t byte, uint32_t& crc32);
	public:
		static uint32_t MemoryCrc32(uint8_t const* mem, const size_t len);

		static uint32_t FileCrc32(wchar_t const* filename);
		static uint32_t FileCrc32(char const* filename);
	};
}


#endif