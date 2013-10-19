// Resource Manager Header File

#pragma once 

#ifndef __RESOURCEMANAGER_H
#define __RESOURCEMANAGER_H

#include <memory>
#include <string>

#include <functional>
#include "IResource.h"
#include "ResourceSystem.h"

namespace Timewhale
{
	class ResourceManager
	{
		friend class twResource;
		friend class ResourceSystem;

		static std::shared_ptr<ResourceManager> rManager;
		
		std::unordered_map<std::size_t, std::function<void(uint8_t*,uint32_t, void*)>> _processing_table;

		ResourceManager();
        ResourceManager(const ResourceManager &);

		uint32_t	_addReference(const std::string& name);
		uint32_t	_addReference(uint32_t tag);

		uint32_t	_decrementReference(const std::string& name);
		uint32_t	_decrementReference(uint32_t tag);

		ResourceSystem* _rs;

		bool _ready;
		bool _loading;

	public:
		/* get
		 * This method returns the pointer to the manager. */
		inline static std::shared_ptr<ResourceManager> const get() {return rManager;}

		/* create
         * This method creates the manager object. */
		static std::shared_ptr<ResourceManager> const create();
		
        ResourceManager &operator= (const ResourceManager &);

		// Load Access Functions
		twResource* Load(const std::string& name, ResourceType type);
		twResource* Load(uint32_t tag, ResourceType type);
		twStream*	LoadStream(const std::string& name, ResourceType type);
		twStream*	LoadStream(uint32_t tag, ResourceType type);
		uint32_t	Async_Load(const std::string& name, ResourceType type);
		uint32_t	Async_Load(uint32_t tag, ResourceType type);

		// Resource Task Accessor
		twResource* GetTask(uint32_t task_id);
		twStream*	GetStream(uint32_t stm_id);

		// Unload Functions
		bool		Unload(const std::string& name);
		bool		Unload(uint32_t tag);

		// Resource Map Augmentation
		bool AddBlub(const std::string& blub_path);
		bool AddManifest(const std::string& mani_path);
		bool AddTextureAtlas(const std::string& atlas_path);

		uint32_t ReadBytes(void* dest, const uint32_t tag, uint32_t count, uint32_t offset);
		
		// Scope Loading and Unloading functions
		bool LoadScope(uint32_t scope);
		bool UnloadScope(uint32_t scope);

		// Misc. Functions
		void setResourceSystem(ResourceSystem* rm);
		bool isReady();
		bool isLoading();
		float PercentLoaded();
	};

	typedef std::shared_ptr<ResourceManager> ResourceManagerPtr;

}

#endif