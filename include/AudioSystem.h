/*
 * Audio System
 * This is the audio system. There are many like it, but this one is mine.
 * It uses the FMOD libraries, which are pretty sick. In the future there
 * will be more features added such as 3d sound positioning and async file
 * loading. Right now it just block loads.
 *
 * As this is an engine system, it is expected that the gameplay devs will
 * not have access to it directly.
*/
#pragma once
#ifndef __Audio_System
#define __Audio_System

#include "fmod\fmod.hpp"
#include "fmod\fmod_errors.h"
#include <unordered_map>
#include <string>
#include <memory>
#include "WhaleMath.h"

#ifdef WIN32
#ifdef _DEBUG
#pragma comment(lib, "fmodexL_vc")
#else
#pragma comment(lib, "fmodex_vc")
#endif
#elif X64
#ifdef _DEBUG
#pragma comment(lib, "fmodexL64_vc")
#elif
#pragma comment(lib, "fmodex64_vc")
#endif
#endif

// Number of channels used by fmod
#define NUM_CHANNELS 128
#define NUM_STREAMS 6

namespace Timewhale {

	struct AudioDetails
	{
		uint32_t _id;
		char*    _data;

		AudioDetails(uint32_t id)
			: _id(id) {}
	};

    class AudioSystem {
        typedef std::unordered_map<uint32_t, FMOD::Sound*> SoundMap;
		typedef std::unordered_map<int, FMOD::Channel*> ChannelMap;

        FMOD::System* mSystem;		//This is the central object of the FMOD library, everything uses it.

        SoundMap mSounds;			//This is a map of sound objects.
		//ChannelMap mUserChannels;   //This is a map of channels used by the system.
        unsigned int mChannelsInUse;//This is number of channels in use.
        unsigned int mVersion;		//This is the version number for FMOD.
        
        static std::shared_ptr<AudioSystem> sSystem;
		static uint32_t audio_UID;
        static float dfactor;

        FMOD_VECTOR mLastPos;

        AudioSystem();
    public:

        static inline std::shared_ptr<AudioSystem> const get() {
            return sSystem;
        }

        static std::shared_ptr<AudioSystem> const create();

        static void setDfactor(float);

		/* init
		 * This method needs to be called to initialize the audio system. */
        bool init();

		/* shutdown 
		 * This method needs to be called during engine shutdown to free the audio resources. */
        void shutdown();

		/* update
		 * This method updates the audio system. */
        void update();

		/* loadStream
		 * This method will stream a song from file using FileStream resource
		 * It will not block to load the whole sound at once, however only one
		 * instance of a stream can be played at once.
		 */
		uint32_t loadStream(const std::string& name);

		/* loadStream
		 * This method will stream a song from file using FileStream resource
		 * It will not block to load the whole sound at once, however only one
		 * instance of a stream can be played at once.
		 */
		uint32_t loadStream(const char* data, uint32_t len);

		/* loadSound
		 * This method will load a sound file. Currently it just block
		 * loads so if you load a big file or something be prepared to
		 * wait a bit. The method returns an id number that should be
		 * saved if you want to play the sound.
		 *\param	string		The filename of the file to be loaded.
		 *\return	int			Returns a id number to reference the sound.	*/
        uint32_t loadSound(const std::string &filename);

		/* loadSound
		 * This method will load a sound file. Currently it just block
		 * loads so if you load a big file or something be prepared to
		 * wait a bit. The method returns an id number that should be
		 * saved if you want to play the sound.
		 *\param	uint8_t*	Pointer to memory to be used to load the sound.
		 *\return	int			Returns a id number to reference the sound.	*/
		uint32_t loadSound(const char* data, uint32_t len);

		/* unloadSound
		 * This method unloads an audio sound and cleans up the associated resources.
		 * The method returns an int success.
		 * param	uint
		 */
		int unloadSound(uint32_t ID);

		/* playSound
		 * This method plays a sound! Later there will be more features
		 * added and options for playing like volume and position and stuff.
		 *\param	int			The id number of the sound to be played.	*/
        int playSound(int soundID);

        int playSound(int soundID, const Point &position);
     
		void pauseSound(int channel_num, bool paused);

		void stopSound(int channel_num);

		float getVolume(int channel_num);

		void setVolume(int channel_num, float volume, bool relative);

        ~AudioSystem();
    };

    typedef std::shared_ptr<AudioSystem> AudioSystemPtr;
}

#endif
