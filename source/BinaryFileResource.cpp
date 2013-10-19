// Binary File Implementation

#include "BinaryFileResource.h"
#include "ResourceManager.h"

namespace Timewhale
{
	twFile::twFile(uint8_t* data, uint32_t len)
		: twResource(), _data(data), _file_size(len), _archive_pos(0)
	{
	}



	uint32_t twFile::_read(void* dest, uint32_t count, uint32_t& pos)
	{
		ResourceManagerPtr rm = ResourceManager::get();
		
		// Ask the rm for bytes from the file, passing in dest and count
		int32_t ovrflw = (count + pos) - _file_size; 

		count -= ovrflw > 0 ? ovrflw : 0;

		memcpy(dest, _data + pos, count);

		pos += count;
		_archive_pos = pos;

		// returning the number of bytes read
		return count;
	}

	WhaleFile::WhaleFile(const std::string& name)
		: IResource(name, BINARY)
	{
		//_file = nullptr;
		//_curr_position = 0;
		if(_file = dynamic_cast<twFile*>(_rsc))
		{
			_curr_position = _file->_archive_pos;
			
		}
		else
		{
			// Error!
		}

	}

	WhaleFile::WhaleFile(uint32_t tag)
		: IResource(tag, BINARY)
	{
		/*_file = nullptr;
		_curr_position = 0;*/
		if(_file = dynamic_cast<twFile*>(_rsc))
		{
			_curr_position = _file->_archive_pos;
		}
		else
		{
			// Error!
		}
	}

	WhaleFile::WhaleFile(const WhaleFile& other)
		: IResource(other)
	{
		*this = other;
	}

	WhaleFile& WhaleFile::operator=(const WhaleFile& other)
	{
		if (&other == this)
			return *this;

		_file = other._file;
		_curr_position = other._curr_position;
		_isReady = other._isReady;

		return *this;
	}

	int WhaleFile::Seek(int32_t offset, FILE_POS origin)
	{
		if(!_isReady)
		{
			if(_getReady())
			{
				_file = dynamic_cast<twFile*>(_rsc);
				_isReady = true;
			}
		}

		switch(origin)
		{
		case WHALE_BEGIN:
			if (offset > (int32_t)(_file->Size()))
				return 1;
			_curr_position = offset;
			break;
		case WHALE_CURR:
			if (_curr_position + offset > _file->Size())
				return 1;
			_curr_position += offset;
			break;
		case WHALE_END:
            if (offset > 0 || _file->Size() + offset < 0)
                return 1;
            else
                _curr_position = _file->Size() + offset;
			//if (_curr_position > _file->Size() || _curr_position < 0)
			//{
			//	_curr_position -= offset;
			//	return 1;
			//}
			break;
		default:
			return 1;
			break;
		}

        _file->_archive_pos = _curr_position;
		return 0;
	}

	uint32_t WhaleFile::Tell()
	{
		if(!_isReady)
		{
			if(_getReady())
			{
				_file = dynamic_cast<twFile*>(_rsc);
				_isReady = true;
			}
		}
		return _curr_position;
	}

	uint32_t WhaleFile::Read(void* dest, uint32_t count)
	{
		if(!_isReady)
		{
			if(_getReady())
			{
				_file = dynamic_cast<twFile*>(_rsc);
				_isReady = true;
			}
		}
		return _file->_read(dest,count, _curr_position);
	}

	uint32_t WhaleFile::Size() {
		if(!_isReady)
		{
			if(_getReady())
			{
				_file = dynamic_cast<twFile*>(_rsc);
				_isReady = true;
			}
		}
		return _file->_file_size;
	}
	uint8_t* WhaleFile::GetMem() {
		if(!_isReady)
		{
			if(_getReady())
			{
				_file = dynamic_cast<twFile*>(_rsc);
				_isReady = true;
			}
		}
		return _file->_data;
	}
}