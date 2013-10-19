// Streaming Audio Resource Header File

#pragma once

#ifndef __AUDIOSTREAM_H
#define __AUDIOSTREAM_H

#include "IResource.h"

namespace Timewhale
{
	class twAudioStream : public twStream
	{
	private:
		uint32_t _audID;
		int		 _channel_ID;

	public:
		twAudioStream(rsc_desc desc);

		bool create_stream();

		void _play_stream();
		void _pause_stream(bool paused);
		void _stop_stream();
	};

	class AudioStream : public ItwStream
	{
	private:
		twAudioStream* _aud;

	public:
		AudioStream(const std::string& name);
		AudioStream(uint32_t tag);

		void Play();
		void Pause(bool paused);
		void Stop();
	};
}

#endif