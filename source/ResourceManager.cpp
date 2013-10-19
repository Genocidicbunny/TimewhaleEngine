// Resource Manager Implementation File

#include "ResourceManager.h"

using namespace Timewhale;
using namespace std;

ResourceManagerPtr ResourceManager::rManager = nullptr;



ResourceManagerPtr const ResourceManager::create()
{
	if (!rManager)
	{
		rManager = ResourceManagerPtr(new ResourceManager());
	}
    return rManager;
}

ResourceManager::ResourceManager()
{
	_ready = false;
	_loading = false;
}

void ResourceManager::setResourceSystem(ResourceSystem* rs)
{
	_rs = rs;

	if (_rs)
		_ready = true;
		
}

bool ResourceManager::isReady()
{
	return _ready;
}

bool ResourceManager::isLoading()
{
	return _rs->_loading;
}

float ResourceManager::PercentLoaded()
{
	return _rs->ratio_done;
}

uint32_t ResourceManager::_addReference(const std::string& name)
{

	return 0;
}

uint32_t ResourceManager::_addReference(uint32_t tag)
{		 
	if(_rs->rsc_map.find(tag) != _rs->rsc_map.end())
	{
		return ++_rs->rsc_map.at(tag)->first._references;
	}

	return 0;
}		 
		 
uint32_t ResourceManager::_decrementReference(const std::string& name)
{	
	return 0;
}		 
uint32_t ResourceManager::_decrementReference(uint32_t tag)
{
	ResourceCount* rsc = nullptr;

	if(_rs->rsc_map.find(tag) != _rs->rsc_map.end())
	{
		rsc = _rs->rsc_map.at(tag);
		rsc->first._references--;
		return rsc->first._references > 0 ? rsc->first._references : 0;
	}

	return 0;
}

twResource* ResourceManager::Load(const std::string& name, ResourceType type)
{
	twResource* result;

	if (_rs != nullptr)
	{
		_rs->loadLock.lock();
		_rs->loadLock.unlock();
		result = _rs->_load(name, type);
	}
	else
	{
		// not fully initialized yet!
	}

	return result;
}

twResource* ResourceManager::Load(uint32_t tag, ResourceType type)
{
	twResource* result;

	if (_rs != nullptr)
	{
		_rs->loadLock.lock();
		_rs->loadLock.unlock();
		result = _rs->_load(tag, type);
	}
	else
	{
		// not fully initialized yet!
	}

	return result;
}

twStream* ResourceManager::LoadStream(const std::string& name, ResourceType type)
{
	twStream* result = nullptr;

	if (_rs != nullptr)
	{
		result = _rs->_load_stream(name, type);
	}

	return result;
}

twStream* ResourceManager::LoadStream(uint32_t tag, ResourceType type)
{
	twStream* result = nullptr;

	if (_rs != nullptr)
	{
		result = _rs->_load_stream(tag, type);
	}

	return result;
}

twStream* ResourceManager::GetStream(uint32_t id)
{
	twStream* result = nullptr;

	if (_rs != nullptr)
	{
		result = _rs->_getStream(id);
	}

	return result;
}

uint32_t ResourceManager::Async_Load(const std::string& name, ResourceType type)
{
	return _rs->_load_async(name, type);
}

uint32_t ResourceManager::Async_Load(uint32_t tag, ResourceType type)
{
	return _rs->_load_async(tag, type);
}

twResource* ResourceManager::GetTask(uint32_t task_id)
{
	return _rs->_getTask(task_id);
}

// MUST RE IMPLEMENT
uint32_t ResourceManager::ReadBytes(void* dest, const uint32_t tag, uint32_t count, uint32_t offset)
{
	uint32_t num = 0;

	if (_rs != nullptr)
	{
		// MUST RE IMPLEMENT
		BlubData data = _rs->_getDataFromBlub(tag, offset, count);
		num = data.second;

		memcpy(dest, data.first, num);
	}
	else
	{
		// not fully initialized yet!
	}

	return num;
}

bool ResourceManager::AddBlub(const std::string& blub_path)
{
	int blub_i = _rs->_blub_vec.size();
	FILE* blub_new = nullptr;
	_rs->_blub_vec.push_back(blub_new);

	if( (_rs->_blub_vec[blub_i] = fopen(blub_path.c_str(), "rb")) == NULL)
	{
		_rs->_blub_vec.pop_back();
		return false;
	}

	bool result = _rs->populateManifestFromBlub(blub_i);
	return result;
}

bool ResourceManager::AddManifest(const std::string& mani_path)
{
	bool result = _rs->populateManifestFromFile(mani_path);
	return result;
}

bool ResourceManager::AddTextureAtlas(const std::string& atlas_path)
{
	bool result = _rs->_add_texture_pack(atlas_path);
	return result;
}

bool ResourceManager::LoadScope(uint32_t scope)
{
	return _rs->_load_scope(scope);
}

bool ResourceManager::UnloadScope(uint32_t scope)
{
	return _rs->_unload_scope(scope);
}