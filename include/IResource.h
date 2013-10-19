// twResource Header File

#pragma once

#ifndef __twResource_H
#define __twResource_H

#include <string>
#include <mutex>
#include <sstream>
#include "ResourceUtil.h"

namespace Timewhale
{
	enum FILE_POS {WHALE_BEGIN = SEEK_SET, WHALE_CURR = SEEK_CUR, WHALE_END = SEEK_END};

	class twResource
	{
		friend class ResourceSystem;

	protected:
		uint32_t	 _tag;
		float		 _timestamp;
		std::string	 _name;

		twResource() : _tag(0), _timestamp(0), _name() {}
		twResource(const std::string& name, uint32_t tag, float time)
			: _name(name), _tag(tag), _timestamp(time) {}

		virtual bool Unload(){return true;}

	public:
		inline uint32_t getTag() {return _tag;}
		uint32_t _add_ref(); // Adds a reference to this resource
		uint32_t _dec_ref(); // Decrements the references to this resource
	};

	class twStream
	{
	private:
	protected:
		std::string  _path;
		std::string  _name;
		std::mutex*  _load_lock;
		int32_t		 _blub;
		uint32_t	 _offset;
		uint32_t	 _size;
		uint32_t	 _tag;
		uint8_t		 _scope;
					 
		uint32_t	 _location;

		virtual bool CloseStream(){return true;}
	public:
		twStream(const rsc_desc& desc);
		twStream(const twStream& other);
		twStream& operator=(const twStream& other);

		uint32_t _read(void* dest, uint32_t count);
		int _seek(int32_t offset, FILE_POS origin);
		uint32_t _tell();
	};


	class IResource
	{
	protected:
		twResource* _rsc;
		IResource(const std::string& name, ResourceType type);
		IResource(uint32_t tag, ResourceType type);
		IResource(const IResource& other);
		IResource& operator=(const IResource& other);
		bool _getReady();
		bool _isReady;
		uint32_t task_id;

	public:
		~IResource();
		uint32_t getTag();
	};

	class ItwStream
	{
	protected:
		twStream* _stm;
		uint32_t _stm_ID;

		ItwStream(const std::string& name, ResourceType type);
		ItwStream(uint32_t tag, ResourceType type);
		ItwStream(const ItwStream& other);
		ItwStream& operator=(const ItwStream& other);

	public:

	};

	class twTextFile : public twResource
	{
	protected:
		std::string _data;

		//virtual bool Unload() override;

	public:
		twTextFile(std::string name);
		std::string Data();
	};
}

#endif