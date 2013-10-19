// Resource Implementation file

#include "IResource.h"
#include "ResourceManager.h"

namespace Timewhale
{
	uint32_t twResource::_add_ref()
	{
		ResourceManagerPtr rm = ResourceManager::get();
		return rm->_addReference(_tag);
	}

	uint32_t twResource::_dec_ref()
	{
		ResourceManagerPtr rm = ResourceManager::get();
		return rm->_decrementReference(_tag);
	}

	twStream::twStream(const rsc_desc& desc)
		: _name(desc._name), _path(desc._path), _load_lock(desc._load_lock),
		  _tag(desc._tag), _blub(desc._blub), _offset(desc._offset), _size(desc._size),
		  _scope(desc._scope), _location(0)
	{
	}

	twStream::twStream(const twStream& other)
	{
		*this = other;
	}

	twStream& twStream::operator=(const twStream& other)
	{
		if (&other == this)
			return *this;
		
		_name = other._name;
		_path = other._path;
		_load_lock = other._load_lock;
		_tag = other._tag;
		_blub = other._blub;
		_offset = other._offset;
		_size = other._size;
		_scope = other._scope;
		_location = other._location;

		return *this;
	}

	uint32_t twStream::_read(void* dest, uint32_t count)
	{
		ResourceManagerPtr rm = ResourceManager::get();

		uint32_t num = rm->ReadBytes(dest, _tag, count, _location);
		_location += num;

		return num;
	}

	int twStream::_seek(int32_t offset, FILE_POS origin)
	{
		switch(origin)
		{
		case WHALE_BEGIN:
			if(offset > _size)
				return 1;
			_location = offset;
			break;
		case WHALE_CURR:
			if(_location + offset > _size)
				return 1;
			_location += offset;
			break;
		case WHALE_END:
			if(offset > 0 || _size + offset < 0)
				return 1;
			_location = _size + offset;
			break;
		default:
			return 1;
			break;
		}
		return 1;
	}

	uint32_t twStream::_tell()
	{
		return _location;
	}

	IResource::IResource(const std::string& name, ResourceType type)
	{
		ResourceManagerPtr rm = ResourceManager::get();

		_isReady = true;
		_rsc = rm->Load(name, type);
		_rsc->_add_ref();
	}

	IResource::IResource(uint32_t tag, ResourceType type)
	{
		ResourceManagerPtr rm = ResourceManager::get();

		_isReady = true;
		_rsc = rm->Load(tag, type);
		_rsc->_add_ref();
	}

	IResource::~IResource()
	{
		if(_rsc != nullptr)
		{
			_rsc->_dec_ref();
			_rsc = nullptr;
		}
	}

	IResource::IResource(const IResource& other)
	{
		*this = other;
	}

	IResource& IResource::operator= (const IResource& other)
	{
		if (&other == this)
			return *this;

		_rsc = other._rsc;
		_rsc->_add_ref();
		_isReady = other._isReady;

		return *this;
	}

	bool IResource::_getReady()
	{
		auto rm = ResourceManager::get();
		if(_rsc == nullptr)
		{
			_rsc = rm->GetTask(task_id);
			_rsc->_add_ref();
		}
		return true;
	}

	uint32_t IResource::getTag()
	{
		if(!_isReady)
		{
			_getReady();
		}
		return _rsc != nullptr ? _rsc->getTag() : 0;
	}

	ItwStream::ItwStream(const std::string& name, ResourceType type)
	{
		auto rm = ResourceManager::get();

		_stm = rm->LoadStream(name, type);
	}
} 