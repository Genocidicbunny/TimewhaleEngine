#pragma once
#ifndef __TIMEWHALE_BLUB_STRUCTS_H_
#define __TIMEWHALE_BLUB_STRUCTS_H_

#include "ResourceUtil.h"

namespace Timewhale {


	union bflags_t{
		struct{
			uint8_t chunk_compression : 1;
			uint8_t unused1			  : 7;
			uint8_t unused2			  : 8;
		};
		uint16_t flags;
	}; 

	union fflags_t {
		struct {
			uint8_t unused1 : 8;
			uint8_t unused2 : 8;
			uint8_t unused3 : 8;
			uint8_t unused4 : 8;
		};
		uint32_t flags;
	};

	struct BlubHeader {
		uint32_t 		blub_sig			:  32;
		uint8_t 		header_size			:   8;
		uint8_t 		version				:   8;
		uint16_t 		flags				:  16;
		uint32_t		blub_size			:  32;
		uint32_t 		toc_size			:  32;
		uint32_t		toc_offset			:  32;
		//more stuff can be added here later
	}; //20 bytes


	struct BlubToCEntry {
		uint32_t		file_size			:  32;
		uint32_t		file_offset			:  32;
		uint32_t		file_crc			:  32;
		uint32_t		file_flags			:  32;
		uint8_t			file_scope			:   8;
		uint8_t			file_type			:	8;
		int8_t			file_nickname[32];
		int8_t			file_name[256];
	}; //304 bytes

	static const size_t BlubToCEntrySize        = sizeof(BlubToCEntry);
	static const size_t	max_files_per_blub 		= (size_t)((~(uint32_t)0) / (sizeof(BlubToCEntry)));
	static const uint8_t max_chunks_per_blub 	= 1;

}

#endif