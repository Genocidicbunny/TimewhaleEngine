// Basic Audio Resource Implementation

#include "BasicAudioResource.h"
#include "AudioSystem.h"
#include "WhaleMath.h"

namespace Timewhale
{
	twBasicAudio::twBasicAudio(uint32_t id, char* data, uint32_t len)
		: _audID(id), _data(data), _len(len)
	{
	}

	int twBasicAudio::_playsound()
	{
		AudioSystemPtr aud_sys = AudioSystem::get();
		return aud_sys->playSound(_audID);
	}

    int twBasicAudio::_playsound(float x, float y)
	{
		AudioSystemPtr aud_sys = AudioSystem::get();
		return aud_sys->playSound(_audID, Point(x, y));
	}

	void twBasicAudio::_stopsound(int channel)
	{
		AudioSystemPtr aud_sys = AudioSystem::get();
		aud_sys->stopSound(channel);
	}

	void twBasicAudio::_pausesound(int channel, bool paused)
	{
		AudioSystemPtr aud_sys = AudioSystem::get();
		//Super_Star super_star; // because of reasons, its late and Im having fun.
		aud_sys->pauseSound(channel, paused);
	}

	void twBasicAudio::_adjustvolume(int channel, float volume, bool relative)
	{
		AudioSystemPtr aud_sys = AudioSystem::get();
		aud_sys->setVolume(channel, volume, relative);
	}

	float twBasicAudio::_getVolume(int channel)
	{
		return AudioSystem::get()->getVolume(channel);
	}

	bool twBasicAudio::Unload()
	{
		auto audio = AudioSystem::get();

		audio->unloadSound(_audID);

		free(_data);
		_audID = 0;
		_len = 0;

		return true;
	}

	BasicAudio::BasicAudio(const std::string& name)
		: IResource(name, AUDIO)
	{
		//_aud = nullptr;
		if(_aud = dynamic_cast<twBasicAudio*>(_rsc))
		{
			// success!
			_curr_channel = -1;
		}
	}

	BasicAudio::BasicAudio(uint32_t tag)
		: IResource(tag, AUDIO)
	{
		//_aud = nullptr;
		if(_aud = dynamic_cast<twBasicAudio*>(_rsc))
		{
			_curr_channel = -1;
		}
	}

	BasicAudio::BasicAudio(const BasicAudio& other)
		: IResource(other)
	{
		*this = other;
	}

	BasicAudio& BasicAudio::operator=(const BasicAudio& other)
	{
		if (&other == this)
			return *this;

		_aud = other._aud;
	
		return *this;
	}

	void BasicAudio::Play()
	{
		if(!_isReady)
		{
			if(_getReady())
			{
				_aud = dynamic_cast<twBasicAudio*>(_rsc);
				_isReady = true;
			}
		}
		_curr_channel = _aud->_playsound();

	}

    void BasicAudio::Play(float x, float y)
	{
		if(!_isReady)
		{
			if(_getReady())
			{
				_aud = dynamic_cast<twBasicAudio*>(_rsc);
				_isReady = true;
			}
		}
		_curr_channel = _aud->_playsound(x, y);
	}

	void BasicAudio::Pause(bool paused)
	{
		if(!_isReady)
		{
			if(_getReady())
			{
				_aud = dynamic_cast<twBasicAudio*>(_rsc);
				_isReady = true;
			}
		}

		_aud->_pausesound(_curr_channel, paused);
	}

	void BasicAudio::Stop()
	{
		if(!_isReady)
		{
			if(_getReady())
			{
				_aud = dynamic_cast<twBasicAudio*>(_rsc);
				_isReady = true;
			}
		}

		_aud->_stopsound(_curr_channel);
	}

	void BasicAudio::SetVolume(float volume)
	{
		_aud->_adjustvolume(_curr_channel, volume, false);
	}

	void BasicAudio::SetVolumeRelative(float relative_volume)
	{
		_aud->_adjustvolume(_curr_channel, relative_volume, true);
	}

	float BasicAudio::GetVolume()
	{
		return _aud->_getVolume(_curr_channel);
	}
}