// Timewhale Resource System Header

#pragma once

#ifndef __RESOURCESYSTEM_H
#define __RESOURCESYSTEM_H

#include <ppltasks.h>
#include <string>
#include <memory>
#include <unordered_map>
#include <mutex>
#include <thread>

#include "ResourceUtil.h"
#include "IResource.h"
#include "RenderSystem.h"
#include "AudioSystem.h"
#include "IAsyncLoad.h"

namespace Timewhale
{
	
	

	typedef std::pair<rsc_desc, twResource*> ResourceCount;
	typedef std::unordered_map<uint32_t, ResourceCount*> ResourceMap;
	typedef std::unordered_map<uint32_t, twStream*> StreamMap;
	typedef std::unordered_map<std::string, uint32_t> NameMap;
	typedef std::vector<ResourceCount*> ScopeVector;
	typedef std::unordered_map<ResourceType, twResource*> ErrorMap;
	typedef std::pair<uint8_t*, uint32_t> BlubData;
	typedef std::unordered_map<uint32_t, concurrency::task<twResource*>> TaskMap;

	class ResourceSystem
	{
		friend class Engine;
		friend class ResourceManager;

	public:
		bool Initialize(std::vector<string> initial_resources);
		void Shutdown();
		void _initialize_errors();
		
		ResourceSystem(void);
		~ResourceSystem(void);

		
	private:
		
	    static TextureDetails createTexture(uint8_t*, uint32_t);
		static AudioDetails   createAudio(uint8_t*, uint32_t);
		static BlubData		  defaultLoader(uint8_t*, uint32_t);
		
		// Member structures
		std::vector<FILE*> _blub_vec;
		ResourceMap		   rsc_map;
		StreamMap		   stm_map;
		ErrorMap		   rsc_err;
		NameMap			   rsc_nam;
		ScopeVector		   rsc_scopes[MAX_SCOPES];
		TaskMap			   tsk_map;
		std::mutex		   blub_access;
		std::mutex		   loadLock;
		bool			   _loading;
		uint32_t		   num_loading;
		float			   ratio_done;

		template<typename T>
		T _processBytes(std::pair<uint8_t*, uint32_t> packet, T(*func)(uint8_t*, uint32_t))
		{
			return func(packet.first, packet.second);
		}

		// Loading functions
		twResource* _load(const std::string& name, ResourceType type);
		twResource* _load(uint32_t tag, ResourceType type);
		twStream*   _load_stream(const std::string& name, ResourceType type);
		twStream*   _load_stream(uint32_t tag, ResourceType type);
		uint32_t	_load_async(const std::string& name, ResourceType type);
		uint32_t	_load_async(uint32_t tag, ResourceType type);
		twResource* _isLoaded(uint32_t tag);
		twResource* _isLoaded(const std::string& name);
		bool		_load_scope(uint32_t scope);
		bool		_unload_scope(uint32_t scope);
		// Load Steps
			// Data Retreival 
			BlubData	_getDataFromBlub(uint32_t tag);
			BlubData	_getDataFromBlub(uint32_t tag, uint32_t offset, uint32_t length);
			BlubData	_getDataFromFile(const std::string& name, ResourceType type);
			BlubData	_loadFileData(FILE* pFile);
			BlubData	_loadFileData(FILE* pFile, uint32_t offset, uint32_t length);
			BlubData	_loadBytesFromBlob(uint32_t off, uint32_t size, uint32_t blub);

			// Data processing
			twResource* _processData(BlubData data, ResourceType type);
			twResource* _load_error(const std::string& name, ResourceType type);

		twStream* _getStream(uint32_t id);
		
		// Manifest Populations Functions
		bool populateManifestFromBlub(uint32_t blub_i);
		bool populateManifestFromFile(const std::string& manifest);
		void registerResource(std::string path, std::string nickname, ResourceType type, uint8_t scope);
		ResourceType getTypeFromString(const char* type);

		bool isRegistered(uint32_t tag);
		bool isRegistered(const std::string& name);

		twResource* _getTask(uint32_t task_id);

		void add_texpack(const std::string& filename);

		//uint32_t _read_bytes(uint8_t* dest, const std::string& name, uint32_t count, uint32_t offset);
		uint32_t _rsc_UID;
		uint32_t _stm_UID;
		uint32_t _task_id;

		bool _add_texture_pack(const std::string& filename);
		std::vector<PackEntry> _parse_pack_manifest(FILE* manifest);
		uint16_t _create_pack(uint8_t*, uint32_t);
		TextureDetails create_pack_texture(PackEntry entry, uint16_t pack_id);
		twResource* _load_error_pack();
	};

	typedef std::shared_ptr<ResourceSystem> ResourceSystemPtr;
}

#endif