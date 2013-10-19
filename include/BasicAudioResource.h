// Basic audio resource header file

#pragma once

#ifndef __BASICAUDIO_H
#define __BASICAUDIO_H

#include "IResource.h"

namespace Timewhale
{
	class twBasicAudio : public twResource
	{
		uint32_t _audID;
		char*	 _data;
		uint32_t _len;

		bool Unload() override;

	public:
		twBasicAudio(uint32_t id, char* data, uint32_t len);
		int _playsound();
        int _playsound(float x, float y);
		void _pausesound(int channel, bool paused);
		void _stopsound(int channel);
		void _adjustvolume(int channel, float volume, bool relative);
		float _getVolume(int channel);
	};

	class BasicAudio : public IResource
	{
		twBasicAudio* _aud;
		int _curr_channel;

	public:
		BasicAudio(const std::string& name);
		BasicAudio(uint32_t tag);
		BasicAudio(const BasicAudio& other);
		BasicAudio& operator=(const BasicAudio& other);

		void Play();
        void Play(float x, float y);
		void Pause(bool paused);
		void Stop();
		void SetVolume(float volume);
		void SetVolumeRelative(float relative_volume);
		float GetVolume();
	};
}

#endif