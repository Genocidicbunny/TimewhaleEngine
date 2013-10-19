// Resource System Implementation File

#include <sstream>

#include <regex>

#include "Engine.h"
#include "ResourceSystem.h"
#include "blub_structs.h"
#include "ResourceManager.h"
#include "TextureResource.h"
#include "BasicAudioResource.h"
#include "BinaryFileResource.h"
#include "AudioStreamResource.h"
#include "WhaleTimer.h"

namespace Timewhale
{
	ResourceSystem::ResourceSystem()
	{
		_rsc_UID = 1;
		_stm_UID = 1;
		_task_id = 1;
		_loading = false;
		num_loading = 0;
	}

	ResourceSystem::~ResourceSystem()
	{
		
	}


	bool ResourceSystem::Initialize(std::vector<string> initial_packs)
	{
		FILE* error_blub = nullptr;
		_blub_vec.clear();
		_blub_vec.push_back(error_blub);

#pragma warning(disable : 4996) //Disable 'this function or variable may be unsafe'
		if( (_blub_vec[0] = fopen("twError.blbr", "rb")) == NULL) 
			return false; // Unable to locate specified blub.

		// Load the error blub, fun stuff
		populateManifestFromBlub(0);

		auto rm = ResourceManager::get();
		
		rm->setResourceSystem(this);

		_initialize_errors();

		rm->AddManifest("UI_Manifest.txt");

		for(int i = 0; i < initial_packs.size(); i++)
		{
			rm->AddTextureAtlas(initial_packs[i]);
		}

		//ResourceManager::get()->AddTextureAtlas("ui_pack");
		//ResourceManager::get()->AddTextureAtlas("Assets/TexturePacks/menu_pack");
		//ResourceManager::get()->AddTextureAtlas("Assets/TexturePacks/level_pack");
		//ResourceManager::get()->AddBlub("whale.blbr");


		// Debug


		return true;
	}

	void ResourceSystem::Shutdown()
	{
		// Unload all current resources
		ResourceMap::iterator it = rsc_map.begin();
		for(; it != rsc_map.end(); it++)
		{
			if(it->second->second)
				it->second->second->Unload();
			delete(it->second->second);
			delete(it->second);
		}

		// Close the blub file
		for(size_t i = 0; i < _blub_vec.size(); i++)
		{
			FILE* _blub = _blub_vec[i];
			if (_blub)
			   fclose(_blub);
		}
		//Need to clean out error resources!
	}

	bool ResourceSystem::populateManifestFromFile(const std::string& manifest_path)
	{
		const char* path = manifest_path.c_str();

		std::ifstream manifest(path);

		if(manifest.is_open())
		{
			// Parse the manifest file and register the resources
			char line[256];
			char* pch;
			char* type;
			char* path;
			ResourceType eType;
			uint8_t scope;

			while(!manifest.eof())
			{
				manifest.getline(line, 256);
				if (line[0] == '#')
					continue;
				pch = strtok(line, " ");

				if(pch == NULL) continue;

				scope = atoi(pch);

				type = strtok(NULL, " ");
				eType = getTypeFromString(type);

				path = strtok(NULL, " ");

				pch = strtok(NULL, " ");

				std::string nick;
				pch != NULL ? nick = std::string(pch) : nick = std::string();

				registerResource(path, nick, eType, scope);
			}
		}
		else
		{
			return false;
		}

		return true;
	}

	void ResourceSystem::registerResource(std::string path, std::string nickname, ResourceType type, uint8_t scope)
	{
		rsc_desc rsc;
		ResourceCount* rsc_cnt;

		rsc._type		= type;
		rsc._offset		= 0;
		rsc._scope		= scope;
		rsc._size		= 0;
		rsc._references = 0;
		rsc._blub		= -1;
		rsc._tag		= _rsc_UID++;
		rsc._path       = path;
		rsc._name       = nickname;
		rsc._hot        = true;
		rsc._dirty      = false;

		rsc_cnt = new ResourceCount(std::make_pair(rsc, nullptr));

		if (nickname.size() > 0)
			rsc_nam.emplace(nickname, rsc._tag);
		else
			rsc_nam.emplace(path, rsc._tag);

		rsc_map.emplace(rsc._tag, rsc_cnt);

		if (rsc._scope < MAX_SCOPES)
		{
			rsc_scopes[rsc._scope].push_back(rsc_cnt);
		}
	}

	bool ResourceSystem::populateManifestFromBlub(uint32_t blub_i)
	{
		FILE* _blub = _blub_vec[blub_i];

		rewind(_blub);
		BlubHeader head;
		BlubToCEntry entry;
		rsc_desc rsc;
		ResourceCount* rsc_cnt;
		std::string rsc_name;
		std::string rsc_path;

		// Allocate Buffers
		char* header_buff = (char*) malloc(sizeof(BlubHeader));

		// Read in the header bytes from blub
		fread(header_buff, sizeof(BlubHeader), 1, _blub);

		// Memcpy the bytes into a BlubHeader
		memcpy(&head, header_buff, sizeof(BlubHeader));

		// Calculate the number of assets in the blub
		assert(BlubToCEntry != 0);
		uint32_t contents_size = head.toc_size / BlubToCEntrySize;

		// allocate the table of contents buffer
		char* tocBuff = (char*) malloc(BlubToCEntrySize * contents_size);

		// Read the table of contents from the blub
		fseek(_blub, head.toc_offset, SEEK_SET);
		fread(tocBuff, BlubToCEntrySize, contents_size, _blub);

		// Loop over the table of contents and parse the information
		for (size_t i = 0; i < contents_size; i++)
		{
			memcpy(&entry, &tocBuff[BlubToCEntrySize * i], BlubToCEntrySize);

			rsc_path = std::string((char*) &entry.file_name);
			rsc_name = std::string((char*) &entry.file_nickname);

			rsc._type		= (ResourceType) entry.file_type;
			rsc._offset		=				 entry.file_offset;
			rsc._scope		=				 entry.file_scope;
			rsc._size		=				 entry.file_size;
			rsc._references =				 0;
			rsc._blub		=				 blub_i;
			rsc._tag		=				 _rsc_UID++;
			rsc._path       =				 rsc_path;
			rsc._name       =                rsc_name;
			rsc._hot        =				 false;
			rsc._dirty      =				 false;

			rsc_cnt = new ResourceCount(std::make_pair(rsc, nullptr));

			if (rsc_name.size() > 0)
				rsc_nam.emplace(rsc_name, rsc._tag);
			else
				rsc_nam.emplace(rsc_path, rsc._tag);

			rsc_map.emplace(rsc._tag, rsc_cnt);

			if (rsc._scope < MAX_SCOPES)
			{
				rsc_scopes[rsc._scope].push_back(rsc_cnt);
			}
		}

		// Free the buffers
		free(header_buff);
		free(tocBuff);		

		// Return happy, because we are happy people
		return true;
	}

	TextureDetails ResourceSystem::createTexture(uint8_t* data, uint32_t len)
	{
		auto rs = RenderSystem::get();
		PackDetails pack_det = rs->CreatePack(data, len);

		TextureDetails tex = rs->CreateTextureDetails(
			TextureDetails(0, 0, 0, pack_det._width, pack_det._height), 
			pack_det._pack_id);
		return tex;
	}

	AudioDetails ResourceSystem::createAudio(uint8_t* data, uint32_t len)
	{
		char* temp = (char*) malloc(len);
		memcpy(temp, data, len);

		AudioDetails aud(Engine::get()->mAudio->loadSound(temp, len));
		aud._data = temp;
		return aud;
	}

	BlubData ResourceSystem::defaultLoader(uint8_t* data, uint32_t len)
	{
		uint8_t* cpy = (uint8_t*) malloc(len);
		memcpy(cpy, data, len);

		return make_pair(cpy, len);
	}

	bool ResourceSystem::_load_scope(uint32_t scope)
	{
		// If scope not within permissable range
		if (scope >= MAX_SCOPES || scope < 0)
			// return false
			return false;

		// Grab scope from rsc_scp
		ScopeVector trgt = rsc_scopes[scope];
		ratio_done = 0;
		if (trgt.size() == 0) return true;

		concurrency::create_task([trgt, this]
		{
			size_t i = 0;
			
			//loadLock.lock();
			_loading = true;
			num_loading = trgt.size();
			ratio_done = 0;


			for(i = 0; i < trgt.size(); i++)
			{
				assert(trgt.size() != 0);
				ratio_done = ((float)i) / trgt.size();

				ResourceCount* rsc = trgt[i];
				// Check to see if rsc is loaded or not
				if(rsc->second != nullptr)
					continue; // If so, continue to the next resource
			
				// Load the resource!!
				_load(rsc->first._tag, rsc->first._type);

				num_loading--;
			}

			//loadLock.unlock();
			_loading = false;
			ratio_done = 1;
		});
		// Iterate through scope, load all null resources

		
		return true;
	}

	bool ResourceSystem::_unload_scope(uint32_t scope)
	{
		// If scope not within permissable range
		if (scope >= MAX_SCOPES || scope < 0)
			// return false
			return false;

		// Grab scope from rsc_scp
		ScopeVector trgt = rsc_scopes[scope];

		// Iterate through scope, load all null resources
		for(size_t i = 0; i < trgt.size(); i++)
		{
			ResourceCount* rsc = trgt[i];
			if(rsc->first._references > 0)
			{
				// continue and maybe log an error?
				log_sxwarn("ResourceSystem", "Resource %s still in use!", rsc->first._name.c_str());
				continue;
			}

			// Here we unload the resource...
			rsc->second->Unload();

			delete(rsc->second);
			rsc->second = nullptr;
		}

		return true;
	}

	twResource* ResourceSystem::_isLoaded(uint32_t tag)
	{
		twResource* result = nullptr;

		if(isRegistered(tag))
		{
			result = rsc_map.at(tag)->second;
		}

		return result;
	}

	twResource* ResourceSystem::_isLoaded(const std::string& name)
	{
		twResource* result = nullptr;

		if(isRegistered(name))
		{
			result = _isLoaded(rsc_nam.at(name));
		}

		return result;
	}

	twResource* ResourceSystem::_load(const std::string& name, ResourceType type)
	{
		uint32_t tag = 0;
		BlubData data = make_pair(nullptr, 0);
		twResource* result = nullptr;
		rsc_desc rsc;		

		// Check to see if we know about this resource
		if(isRegistered(name)) // If yes, check type, call get data from blub, and assign tag.
		{
			tag = rsc_nam.at(name);
			rsc = rsc_map.at(tag)->first;

			rsc._load_lock->lock();

			// If the resource is loaded, return it
			if(result = _isLoaded(name))
			{
				rsc._load_lock->unlock();
				return result;
			}

			// check for type mismatch, ignore binary case, any file can be loaded into raw bytes
			if (type != BINARY && type != rsc_map.at(tag)->first._type)
			{
				rsc._load_lock->unlock();
				return rsc_err.at(type);
			}
			// Get data from file or from hot directory
			data = _getDataFromBlub(tag);
		}
		else // Else treat the name as a path and attempt to load
		{
			//data =_getDataFromFile(name,type);
			log_sxerror("ResourceSystem", "No resource registered by %s", name.c_str());
			return rsc_err.at(type);
		}

		// If data is still null then we encountered an error
		if(data.first == nullptr)
		{
			// return error of type
			if (rsc_err.find(type) != rsc_err.end())
			{
				log_sxerror("ResourceSystem", "Returning error resource for %s", name.c_str());
				rsc._load_lock->unlock();
				return rsc_err.at(type);
			}
			else
			{
				log_sxerror("ResourceSystem", "No Error Resource for type: %d!!", type);
				rsc._load_lock->unlock();
				return nullptr;
			}
		}

		// Otherwise, process the data that we have
		result = _processData(data, type);

		tag = rsc_nam.at(name); // Re grab tag in case the resource was only just now registered. 
		result->_tag = tag;
		result->_name = name;
		result->_timestamp = WhaleTimer::getCurrentTime();

		free(data.first);

		// Store resource in rsc_map
		rsc_map.at(tag)->second = result;
		rsc._load_lock->unlock();

		return result;
	}

	// TODO awesome hot loading!!
	twResource* ResourceSystem::_load(uint32_t tag, ResourceType type)
	{
		BlubData data = make_pair(nullptr, 0);
		twResource* result = nullptr;
		rsc_desc rsc;

		

		// Check to see if we know about this resource
		if(isRegistered(tag)) // If yes, check type, call get data from blub, and assign tag.
		{
			rsc = rsc_map.at(tag)->first;
			
			// Obtain the resource lock
			rsc._load_lock->lock();
			
			// If the resource is loaded, return it
			if(result = _isLoaded(tag))
			{
				rsc._load_lock->unlock();
				return result;
			}

			// check for type mismatch, ignore binary case, any file can be loaded into raw bytes
			if (type != BINARY && type != rsc_map.at(tag)->first._type)
			{
				rsc._load_lock->unlock();
				return rsc_err.at(type);
			}
			// Get data from file or from hot directory
			data = _getDataFromBlub(tag);
		}
		else // We have no knowledge of this place
		{
			// return error of type
			if (rsc_err.find(type) != rsc_err.end())
			{
				log_sxwarn("ResourceSystem", "Returning error resource");
				return rsc_err.at(type);
			}
			else
			{
				log_sxerror("ResourceSystem", "No Error Resource for type: %d!!", type);
				return nullptr;
			}
		}

		// If data is still null then we encountered an error
		if(data.first == nullptr)
		{
			if (rsc_err.find(type) != rsc_err.end())
			{
				log_sxwarn("ResourceSystem", "Returning error resource");
				rsc._load_lock->unlock();
				return rsc_err.at(type);
			}
			else
			{
				log_sxerror("ResourceSystem", "No Error Resource for type: %d!!", type);
				rsc._load_lock->unlock();
				return nullptr;
			}
		}

		// Otherwise, process the data that we have
		result = _processData(data, type);

		result->_tag = tag;
		result->_name = rsc._name;
		result->_timestamp = WhaleTimer::getCurrentTime();

		free(data.first);

		// Store resource in rsc_map
		rsc_map.at(tag)->second = result;
		rsc._load_lock->unlock();

		return result;
	}

	twStream* ResourceSystem::_load_stream(const std::string& name, ResourceType type)
	{
		twStream* strm = nullptr;
		uint32_t id = 0;
		rsc_desc rsc;
		uint32_t tag = 0;

		if(isRegistered(name))
		{
			tag = rsc_nam.at(name);

			if(stm_map.find(tag) != stm_map.end())
			{
				return stm_map.at(tag);
			}

			rsc = rsc_map.at(tag)->first;

			if(rsc._type != type)
			{
				// TODO: Default Stream Pointer!
				log_sxerror("ResourceSystem", "Stream attempted for resource %s is of mismatched type", name.c_str());
			}
			id = _stm_UID++;
			
		}
		else
		{
			// No such resource found
		}

		switch(type)
		{
		case AUDIO:
		{
			twAudioStream* temp = new twAudioStream(rsc);
			strm = dynamic_cast<twStream*>(temp);
		}
			break;
		default:
			strm = new twStream(rsc);
			break;
		}

		stm_map.emplace(tag, strm);

		return strm;
	}

	twStream* ResourceSystem::_load_stream(uint32_t tag, ResourceType type)
	{
		twStream* strm = nullptr;
		uint32_t id = 0;
		rsc_desc rsc;

		if(isRegistered(tag))
		{
			if(stm_map.find(tag) != stm_map.end())
			{
				return stm_map.at(tag);
			}

			rsc_desc rsc = rsc_map.at(tag)->first;

			if(rsc._type != type)
			{
				// TODO: Default Stream Pointer!
				log_sxerror("ResourceSystem", "Stream attempted for resource %d is of mismatched type", tag);
			}
		}
		else
		{
			// No such resource found
		}

		switch(type)
		{
		case AUDIO:
		{
			twAudioStream* temp = new twAudioStream(rsc);
			strm = dynamic_cast<twStream*>(temp);
		}
			break;
		default:
			strm = new twStream(rsc);
			break;
		}


		return strm;
	}

	twStream* ResourceSystem::_getStream(uint32_t id)
	{
		if (stm_map.find(id) != stm_map.end())
		{
			return stm_map.at(id);
		}

		return nullptr;
	}

	uint32_t ResourceSystem::_load_async(const std::string& name, ResourceType type)
	{
		uint32_t task_id = _task_id++;

		concurrency::task<twResource*> task;

		task = concurrency::create_task([this, name, type]
		{
			return _load(name, type);
		});
		
		tsk_map.emplace(task_id, task);

		return task_id;
	}

	uint32_t ResourceSystem::_load_async(uint32_t tag, ResourceType type)
	{
		uint32_t task_id = _task_id++;

		concurrency::task<twResource*> task;

		task = concurrency::create_task([this, tag, type]
		{
			return _load(tag, type);
		});

		tsk_map.emplace(task_id, task);

		return task_id;
	}

	twResource* ResourceSystem::_getTask(uint32_t task_id)
	{
		twResource* result = nullptr;

		if(tsk_map.find(task_id) != tsk_map.end())
		{
			concurrency::task<twResource*> _task;
			_task = tsk_map.at(task_id);
			result = _task.get();
			tsk_map.erase(task_id);
		}

		return result;
	}

	BlubData ResourceSystem::_getDataFromBlub(uint32_t tag)
	{
		BlubData data = make_pair(nullptr, 0);
		FILE* hot = nullptr;
		if(isRegistered(tag)) // We have a record of this resource in the blub manifest
		{
			rsc_desc rsc = rsc_map.at(tag)->first;

			blub_access.lock();
			// Attempt to hotload the resource (if mode is debug?) (check timestamp on blub vs hotload?)
			if((hot = fopen(rsc._path.c_str(), "rb")) != NULL)
			{
				data = _loadFileData(hot);
			}
			else if(rsc._blub != -1) // If that fails, load from blub
			{
				data = _loadBytesFromBlob(rsc._offset, rsc._size, rsc._blub);
			}
			else
			{
				log_sxerror("ResourceSystem", "Missing Asset: %s!", rsc._name.c_str());
			}
			blub_access.unlock();
		}

		return data;
	}

	BlubData ResourceSystem::_getDataFromBlub(uint32_t tag, uint32_t offset, uint32_t length)
	{
		BlubData data = make_pair(nullptr, 0);
		FILE* hot = nullptr;
		if(isRegistered(tag)) // We have a record of this resource in the blub manifest
		{
			rsc_desc rsc = rsc_map.at(tag)->first;

			blub_access.lock();
			// Attempt to hotload the resource (if mode is debug?) (check timestamp on blub vs hotload?)
			if((hot = fopen(rsc._path.c_str(), "rb")) != NULL)
			{
				data = _loadFileData(hot, offset, length);
			}
			else if(rsc._blub != -1) // If that fails, load from blub
			{
				data = _loadBytesFromBlob(rsc._offset + offset, length, rsc._blub);
			}
			else
			{
				log_sxerror("ResourceSystem", "Missing Asset: %s!", rsc._name.c_str());
			}
			blub_access.unlock();
		}

		return data;
	}

	BlubData ResourceSystem::_getDataFromFile(const std::string& name, ResourceType type)
	{
		BlubData data = make_pair(nullptr, 0);
		FILE* hot = nullptr;

		if((hot = fopen(name.c_str(), "rb")) != NULL)
		{
			data = _loadFileData(hot);
			rsc_desc rsc;
			rsc._type		=	type;
			rsc._offset		=	0;
			rsc._scope		=	0;
			rsc._size		=	data.second;
			rsc._references =	0;
			rsc._tag		=	_rsc_UID++;
			rsc._path       =	name;
			rsc._name		=   name;
			rsc._hot        =	true;
			rsc._dirty      =   false;
			rsc._loading    =   false;

			rsc_map.emplace(make_pair(rsc._tag, new ResourceCount(rsc, nullptr)));
			rsc_nam.emplace(make_pair(name, rsc._tag));
		}
		else // not a valid path, so we dont know what the user is talking about what-so-ever
		{
			// Return null data 
			log_sxwarn("ResourceSystem", "Resource cannot be determined for %s.", name.c_str());
		}
		return data;
	}

	twResource* ResourceSystem::_processData(BlubData data, ResourceType type)
	{
		twResource* result = nullptr;

		switch(type)
		{
		case TEXTURE:
			{
				TextureDetails tex_det =_processBytes<TextureDetails>(data, ResourceSystem::createTexture);
				twTexture* tex = new twTexture(tex_det._tex_id, tex_det._width, tex_det._height);
				result = dynamic_cast<twResource*>(tex);
			}
			break;
		case AUDIO:
			{
				AudioDetails aud_det = _processBytes<AudioDetails>(data, ResourceSystem::createAudio);
				twBasicAudio* aud = new twBasicAudio(aud_det._id, aud_det._data, data.second);
				result = dynamic_cast<twResource*>(aud);
			}
			break;
		case BINARY:
			{
				BlubData file_dat = _processBytes<BlubData>(data, ResourceSystem::defaultLoader);
				twFile* file = new twFile(file_dat.first, file_dat.second);
				result = dynamic_cast<twResource*>(file);
			}
			break;
		case OTHER:
			{
				BlubData file_dat = _processBytes<BlubData>(data, ResourceSystem::defaultLoader);
				twFile* file = new twFile(file_dat.first, file_dat.second);
				result = dynamic_cast<twResource*>(file);
			}
			break;
		default:
			break;
		}

		return result;
	}

	void ResourceSystem::_initialize_errors()
	{
		// Load and initialize the default error resources
		rsc_err.emplace(make_pair(TEXTURE, _load_error_pack()));
        rsc_err.emplace(make_pair(BINARY, _load_error("FILETEST", BINARY)));
		rsc_err.emplace(make_pair(AUDIO, _load_error("boo", AUDIO)));
	}

	twResource* ResourceSystem::_load_error(const std::string& name, ResourceType type)
	{
		uint32_t tag = rsc_nam.at(name);
		BlubData data = _getDataFromBlub(tag);
        return _processData(data, type);
	}

	twResource* ResourceSystem::_load_error_pack()
	{
		twResource* result = nullptr;
		_add_texture_pack("twError");

		result = _isLoaded("FailWhale");

		return result;
	}

	bool ResourceSystem::isRegistered(const std::string& name)
	{
		return rsc_nam.find(name) != rsc_nam.end();
	}

	bool ResourceSystem::isRegistered(uint32_t tag)
	{
		return rsc_map.find(tag) != rsc_map.end();
	}

	BlubData ResourceSystem::_loadBytesFromBlob(uint32_t off, uint32_t size, uint32_t blub)
	{
		FILE* _blub = _blub_vec[blub];
		uint8_t* buff = (uint8_t*) malloc(size);
		fseek(_blub, off, SEEK_SET);
		fread(buff, size, 1, _blub);

		return std::make_pair(buff, size);
	}

	BlubData ResourceSystem::_loadFileData(FILE* pFile)
	{
		uint32_t buffSize;
		uint8_t* data;
		size_t result;

		fseek(pFile, 0, SEEK_END);
		buffSize = ftell(pFile);
		rewind(pFile);

		data = (uint8_t*) calloc (buffSize + 1, sizeof(uint8_t));
		if (data == NULL)
		{
			// OUT OF MEMORY!!!
			//free(data);
			return make_pair(nullptr, 0);
		}

		result = fread(data, sizeof(uint8_t), buffSize, pFile);
		if(result != buffSize)
		{
			// Error reading data
			free(data);
			return make_pair(nullptr, 0);
		}

		return make_pair(data, buffSize);
	}

	BlubData ResourceSystem::_loadFileData(FILE* pFile, uint32_t offset, uint32_t length)
	{
		uint8_t* data;
		size_t result;

		data = (uint8_t*) calloc (length + 1, sizeof(uint8_t));
		if (data == NULL)
		{
			// OUT OF MEMORY!!!
			//free(data);
			return make_pair(nullptr, 0);
		}

		fseek(pFile, offset, SEEK_SET);

		result = fread(data, sizeof(uint8_t), length, pFile);
		if(result != length)
		{
			// Error reading data
			free(data);
			return make_pair(nullptr, 0);
		}

		return make_pair(data, length);
	}

	ResourceType ResourceSystem::getTypeFromString(const char* type)
	{
		static struct 
		{
			const char *s;
			ResourceType rt;
		} map[] = {
			{ "TEXTURE", TEXTURE},
			{ "AUDIO", AUDIO },
			{ "BINARY", BINARY},
			{ "OTHER", OTHER },
		};

		int i;
		for ( i = 0 ; i < sizeof(map)/sizeof(map[0]); i++ ) {
			if ( strcmp(type, map[i].s) == 0 ) {
				return map[i].rt;
			}
		}

		return OTHER;
	}

	bool ResourceSystem::_add_texture_pack(const std::string& filename)
	{
		FILE* texture_pack = nullptr;
		FILE* pack_manifest = nullptr;
		BlubData texture_data;
		uint16_t packID = 0;
		std::vector<PackEntry> pack_entries;
		std::string pack_filename = filename;
		pack_filename.append(".png");
		std::string mani_filename = filename;
		mani_filename.append(".txt");
	
		// Load texture, give to adam
		// create and open file
		if((texture_pack = fopen(pack_filename.c_str(), "rb")) != NULL)
		{
			// get data from file
			texture_data = _loadFileData(texture_pack);
			// call create pack
			packID = _create_pack(texture_data.first, texture_data.second);

			if (packID == 0)
			{
				// Error!
				return false;
			}

			fclose(texture_pack);
			free(texture_data.first);

			// open manifest
			if((pack_manifest = fopen(mani_filename.c_str(), "rb")) != NULL)
			{
				pack_entries = _parse_pack_manifest(pack_manifest);

				fclose(pack_manifest);
			}
			else
			{
				// Error
				return false;
			}
		}
		else
		{
			// Error
			return false;
		}

		for(int i = 0; i < pack_entries.size(); i++)
		{
			PackEntry curr = pack_entries[i];

			// create rsc_desc
			rsc_desc rsc;
			rsc._type		=	TEXTURE;
			rsc._offset		=	0;
			rsc._scope		=	packID;
			rsc._size		=	0;
			rsc._references =	0;
			rsc._tag		=	_rsc_UID++;
			rsc._path       =	curr.Name;
			rsc._name		=   curr.Name;
			rsc._hot        =	false;
			rsc._dirty      =   false;
			rsc._loading    =   false;

			// Send info to adam, get back texture details!
			TextureDetails tex_det = create_pack_texture(curr, packID);

			log_sxinfo("ResourceSystem", "Loaded texture named %s with id %d", curr.Name.c_str(), tex_det._tex_id);

			// create twTexture
			twTexture* texture = new twTexture(tex_det._tex_id, tex_det._width, tex_det._height);

			// emplace
			rsc_map.emplace(make_pair(rsc._tag, new ResourceCount(rsc, dynamic_cast<twResource*>(texture))));
			rsc_nam.emplace(make_pair(curr.Name, rsc._tag));

			// party
		}

		return true;
	}

	uint16_t ResourceSystem::_create_pack(uint8_t* data, uint32_t len)
	{
		// Call adam's function!
		auto rs = RenderSystem::get();
		return rs->CreatePack(data, len)._pack_id;
	}

	std::vector<PackEntry> ResourceSystem::_parse_pack_manifest(FILE* manifest)
	{
		std::vector<PackEntry> result;
		std::regex prime_reg("\"([a-zA-Z0-9_]+)\.[a-zA-Z]+\":\n");
		std::regex reg(".*:([0-9]+).*:([0-9]+).*:([0-9]+).*:([0-9]+)");

		std::string name;

		if(manifest == nullptr)
			return result;

		fseek(manifest, 0, SEEK_END);
		uint32_t len = ftell(manifest);
		rewind(manifest);

		char* manifest_data = (char*) calloc (len+1, sizeof(char));
		if (manifest_data == nullptr)
			return result;

		int numRead = fread(manifest_data, sizeof(char), len, manifest);
		if (numRead != len)
		{
			free(manifest_data);
			return result;
		}

		std::string manifest_string(manifest_data);
		free(manifest_data);
		manifest_data = nullptr;

		std::string pack_string = manifest_string;
		std::smatch matches;

		while(std::regex_search(pack_string, matches, prime_reg))
		{
			name = matches[1];

			pack_string = matches.suffix();

			std::regex_search(pack_string, matches, reg);

			// x : matches[1], y : matches[2], w : matches[3], h : matches[4] 
			PackEntry curr(name, matches[1], matches[2], matches[3], matches[4]);

			result.push_back(curr);

			pack_string = matches.suffix();
		}


		return result;
	}

	TextureDetails ResourceSystem::create_pack_texture(PackEntry entry, uint16_t pack_id)
	{
		auto rs = RenderSystem::get();
		return rs->CreateTextureDetails(TextureDetails(0, entry.XPos, entry.YPos, entry.Width, entry.Height), pack_id);
	}

	//uint32_t ResourceSystem::_read_bytes(uint8_t* dest, const std::string& name, uint32_t count, uint32_t offset)
	//{
	//	BlubData data = _getData(name, BINARY);
	//	
	//	uint32_t ovrflw = (count + offset) - data.second; 

	//	count -= ovrflw > 0 ? ovrflw : 0;

	//	memcpy(dest, data.first + offset, count);

	//	free(data.first);
	//	
	//	return count;
	//}

}