// Audio Stream Implementation File!

#include "AudioStreamResource.h"
#include "AudioSystem.h"
#include "TWLogger.h"

namespace Timewhale
{
	twAudioStream::twAudioStream(rsc_desc desc)
		: twStream(desc)
	{
		_audID = 0;
		_channel_ID = 0;

		create_stream();
	}

	bool twAudioStream::create_stream()
	{
		if(_audID == 0)
		{
			AudioSystemPtr aud = AudioSystem::get();
			_audID = aud->loadStream(_path);
			return true;
		}
		else
		{
			log_sxerror("ResourceSystem", "Cannot create stream for %s. It is already streaming!", _name.c_str());
		}

		return false;
	}

	void twAudioStream::_play_stream()
	{
		if (_audID != 0)
		{
			AudioSystemPtr aud = AudioSystem::get();
			_channel_ID = aud->playSound(_audID);
		}
	}

	void twAudioStream::_pause_stream(bool paused)
	{
		if (_audID != 0)
		{
			AudioSystemPtr aud = AudioSystem::get();
			aud->pauseSound(_channel_ID, paused);
		}
	}

	void twAudioStream::_stop_stream()
	{
        if (_audID != 0)
        {
            AudioSystemPtr aud = AudioSystem::get();
            aud->stopSound(_channel_ID);
        }
	}

	AudioStream::AudioStream(const std::string& name)
		: ItwStream(name, AUDIO)
	{
		if (_aud = dynamic_cast<twAudioStream*>(_stm))
		{

		}
	}

	void AudioStream::Play()
	{
		if(_aud != nullptr)
		{
			_aud->_play_stream();
		}
	}

	void AudioStream::Pause(bool paused)
	{
		if(_aud != nullptr)
		{
			_aud->_pause_stream(paused);
		}
	}

	void AudioStream::Stop()
	{
        if(_aud != nullptr)
        {
            _aud->_stop_stream();
        }
	}
}