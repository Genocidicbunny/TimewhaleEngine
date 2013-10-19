
#include "AudioSystem.h"
#include "AudioStreamResource.h"
#include "TWLogger.h"
#include "ProfileSample.h"
#include "ResourceManager.h"
#include "SceneManager.h"
#include "Camera.h"

#include <functional>
#include <string>

using namespace std;
using namespace FMOD;
using namespace Timewhale;

uint32_t AudioSystem::audio_UID = 0;

const float DISTANCEFACTOR = 1.0f;
const int   INTERFACE_UPDATETIME = 50;

float AudioSystem::dfactor = 0.001f;

void AudioSystem::setDfactor(float val) {
    dfactor *= val;
}

bool ERRCHECK(FMOD_RESULT result, int line)
{
    if (result != FMOD_OK)
    {
        log_sxerror("Audio", "Line: %d, FMOD error! (%d) %s", line, result, FMOD_ErrorString(result));
        return false;
    }
    return true;
}

AudioSystemPtr AudioSystem::sSystem = nullptr;

AudioSystemPtr const AudioSystem::create() {
    if (!sSystem) {
        sSystem = AudioSystemPtr(new AudioSystem());
    }
    return sSystem;
}

AudioSystem::AudioSystem() :
    mSystem(nullptr),
    mVersion(-1)
{
    mLastPos.x = 0.0f;
    mLastPos.y = 0.0f;
    mLastPos.z = 0.0f;
}

AudioSystem::~AudioSystem() {
    
}

bool AudioSystem::init() {
    FMOD_RESULT result;
    int numDrivers;
    FMOD_CAPS caps;
    FMOD_SPEAKERMODE speakerMode;
    char name[256];

    result = System_Create(&mSystem);
    if (!ERRCHECK(result, __LINE__)) return false;

    result = mSystem->getVersion(&mVersion);
    if (!ERRCHECK(result, __LINE__)) return false;

    if (mVersion < FMOD_VERSION) {
        log_sxerror(
            "Audio", 
            "Error!  You are using an old version of FMOD %08x.  This program requires %08x\n", 
            mVersion, FMOD_VERSION);
        return false;
    }

    result = mSystem->getNumDrivers(&numDrivers);
    if (!ERRCHECK(result, __LINE__)) return false;

    if (numDrivers == 0) {
        result = mSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
        if (!ERRCHECK(result, __LINE__)) return false;
    }
    else {
        result = mSystem->getDriverCaps(0, &caps, 0, &speakerMode);
        if (!ERRCHECK(result, __LINE__)) return false;

        result = mSystem->setSpeakerMode(speakerMode);
        if (!ERRCHECK(result, __LINE__)) return false;

        if (caps & FMOD_CAPS_HARDWARE_EMULATED)
        {
            result = mSystem->setDSPBufferSize(1024, 10);
            if (!ERRCHECK(result, __LINE__)) return false;
        }

        result = mSystem->getDriverInfo(0, name, 256, 0);
        if (!ERRCHECK(result, __LINE__)) return false;

        if (strstr(name, "SigmaTel"))
        {
            result = mSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0, 0, FMOD_DSP_RESAMPLER_LINEAR);
            if (!ERRCHECK(result,__LINE__)) return false;
        }
    }

    result = mSystem->init(NUM_CHANNELS, FMOD_INIT_NORMAL, nullptr);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER) {
        result = mSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        if (!ERRCHECK(result,__LINE__)) return false;

        result = mSystem->init(32, FMOD_INIT_NORMAL, 0);
        if (!ERRCHECK(result, __LINE__)) return false;
    }

	//result = mSystem->setFileSystem(AudioSystem::myopen, AudioSystem::myclose, AudioSystem::myread, AudioSystem::myseek, AudioSystem::myasyncread, AudioSystem::myasynccancel, 2048);

    result = mSystem->set3DSettings(1.0f, DISTANCEFACTOR, 1.0f);
    if (!ERRCHECK(result, __LINE__)) return false;

    return true;
}

void AudioSystem::shutdown() {
    FMOD_RESULT result;

    auto itor = mSounds.begin();
    auto end = mSounds.end();

    for(; itor != end; ++itor) {
        result = (*itor).second->release();
        ERRCHECK(result, __LINE__);
    }

    result = mSystem->close();
    ERRCHECK(result, __LINE__);
}

void AudioSystem::update() {
    PROFILE("Audio");
    FMOD_VECTOR forward = { 0.0f, 0.0f, 1.0f };
    FMOD_VECTOR up = { 0.0f, 1.0f, 0.0f };
    FMOD_VECTOR listener, velocity;

    auto camera = SceneManager::GetCurrentScene()->GetCamera();
    camera->GetPosition(listener.x, listener.y);
    listener.x *= -dfactor;
    listener.y *= dfactor;
    listener.z = 0.0f;

    velocity.x = (listener.x - mLastPos.x) * (1000 / INTERFACE_UPDATETIME);
    velocity.y = (listener.y - mLastPos.y) * (1000 / INTERFACE_UPDATETIME);
    velocity.z = 0.0f;

    mLastPos = listener;
  
    mSystem->set3DListenerAttributes(0, &listener, &velocity, &forward, &up);

    mSystem->update();
}


uint32_t AudioSystem::loadStream(const std::string& name)
{
	if (!mSystem) {
        log_sxerror("Audio", "System not initialized! Cannot load sounds!");
        return 0;
    }
    
    FMOD_RESULT result;
    Sound* sound;
	FMOD_CREATESOUNDEXINFO settings;
	memset(&settings, 0, sizeof(FMOD_CREATESOUNDEXINFO));

	settings.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
	//settings.useropen = AudioSystem::myopen;

	result = mSystem->createStream(name.c_str(), FMOD_SOFTWARE | FMOD_LOOP_NORMAL | FMOD_2D, nullptr, &sound);
	if (!ERRCHECK(result, __LINE__)) return 0;

	result = sound->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
    if (!ERRCHECK(result, __LINE__)) return 0;

	uint32_t id = ++audio_UID;
    mSounds.insert(make_pair(id, sound));
    return id;
}

uint32_t AudioSystem::loadStream(const char* data, uint32_t len)
{
	if (!mSystem) {
        log_sxerror("Audio", "System not initialized! Cannot load sounds!");
        return 0;
    }

	FMOD_RESULT result;
	Sound* sound;
	FMOD_CREATESOUNDEXINFO settings;

	memset(&settings, 0, sizeof(FMOD_CREATESOUNDEXINFO));	
	settings.length = len;
	settings.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);

	result = mSystem->createStream(data, FMOD_SOFTWARE | FMOD_3D | FMOD_OPENMEMORY, &settings, &sound);
    if (!ERRCHECK(result, __LINE__)) return 0;

    result = sound->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
    if (!ERRCHECK(result, __LINE__)) return 0;

	uint32_t id = ++audio_UID;
    mSounds.insert(make_pair(id, sound));
    return id;
}

uint32_t AudioSystem::loadSound(const string &filename) {
    if (!mSystem) {
        log_sxerror("Audio", "System not initialized! Cannot load sounds!");
        return 0;
    }
    
    FMOD_RESULT result;
    Sound* sound;

    //result = mSystem->createSound(filename.c_str(), FMOD_HARDWARE, 0, &sound);
    result = mSystem->createSound(filename.c_str(), FMOD_SOFTWARE | FMOD_3D , nullptr, &sound);
    if (!ERRCHECK(result, __LINE__)) return 0;

    result = sound->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
    if (!ERRCHECK(result, __LINE__)) return 0;

	uint32_t id = ++audio_UID;
    mSounds.insert(make_pair(id, sound));
    return id;
}

uint32_t AudioSystem::loadSound(const char* data, uint32_t len)
{
	if (!mSystem) {
        log_sxerror("Audio", "System not initialized! Cannot load sounds!");
        return 0;
    }

	FMOD_RESULT result;
	Sound* sound;
	FMOD_CREATESOUNDEXINFO settings;

	memset(&settings, 0, sizeof(FMOD_CREATESOUNDEXINFO));	
	settings.length = len;
	settings.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);

	result = mSystem->createSound(data, FMOD_SOFTWARE | FMOD_3D | FMOD_OPENMEMORY, &settings, &sound);
    if (!ERRCHECK(result, __LINE__)) return 0;

    result = sound->set3DMinMaxDistance(0.5f * DISTANCEFACTOR, 5000.0f * DISTANCEFACTOR);
    if (!ERRCHECK(result, __LINE__)) return 0;

	uint32_t id = ++audio_UID;
    mSounds.insert(make_pair(id, sound));
    return id;
}

int AudioSystem::unloadSound(uint32_t ID)
{
	if (!mSystem) {
        log_sxerror("Audio", "System not initialized! Cannot unload sounds!");
        return 0;
    }

	//FMOD_RESULT result;
	//Sound* sound;

	if (mSounds.find(ID) != mSounds.end())
	{
		//sound = mSounds.at(ID);
		//if(sound != nullptr)
		//{
		//	result = sound->release();
		//	if (!ERRCHECK(result, __LINE__)) return 0;
		//}

		mSounds.erase(ID);
	}
	else
	{
		log_sxerror("Audio", "No sound associated with ID: %d", ID);
		return 0;
	}

	//temporary return
	return 1;
}

int AudioSystem::playSound(int soundID) {
    FMOD_RESULT result;
	int channel_num = -1;

    if (!(mSounds.count(soundID) > 0)) {
        log_sxerror("Audio", "Invalid sound id!");
        return -1;
    }
    
    mSounds.at(soundID)->setMode(FMOD_2D);

	Channel* channel;
    //result = mSystem->getChannel(FMOD_CHANNEL_FREE, &channel);
    //ERRCHECK(result, __LINE__);

    result = mSystem->playSound(
        FMOD_CHANNEL_REUSE,
        mSounds.at(soundID),
        false,
        &channel);
    ERRCHECK(result, __LINE__);

    channel->getIndex(&channel_num);
	ERRCHECK(result, __LINE__);

	return channel_num;
}

int AudioSystem::playSound(int soundID, const Point &position) {
    FMOD_RESULT result;
    FMOD_VECTOR pos = { position.x * dfactor, position.y * dfactor, 0.0f };
    FMOD_VECTOR vel = { 0.0f, 0.0f, 0.0f };
	int channel_num = -1;

    if (!(mSounds.count(soundID) > 0)) {
        log_sxerror("Audio", "Invalid sound id!");
        return -1;
    }

    mSounds.at(soundID)->setMode(FMOD_3D);

    Channel* channel;
    //result = mSystem->getChannel(FMOD_CHANNEL_FREE, &channel);
    //ERRCHECK(result, __LINE__);
    
    result = mSystem->playSound(FMOD_CHANNEL_REUSE, mSounds.at(soundID), true, &channel);
    ERRCHECK(result, __LINE__);

    result = channel->set3DAttributes(&pos, &vel);
    ERRCHECK(result, __LINE__);

    result = channel->setPaused(false);
	ERRCHECK(result, __LINE__);

	result = channel->getIndex(&channel_num);
	ERRCHECK(result, __LINE__);

	return channel_num;
}

void AudioSystem::pauseSound(int channel_num, bool paused)
{
	FMOD_RESULT result;
	Channel* channel;

	result = mSystem->getChannel(channel_num, &channel);
	if(!ERRCHECK(result, __LINE__))
		return;

	result = channel->setPaused(paused);
	ERRCHECK(result, __LINE__);
}

void AudioSystem::stopSound(int channel_num)
{
	FMOD_RESULT result;
	Channel* channel;

	result = mSystem->getChannel(channel_num, &channel);
	if(!(ERRCHECK(result, __LINE__)))
		return;

	result = channel->stop();

	ERRCHECK(result, __LINE__);
}

float AudioSystem::getVolume(int channel_num)
{
	FMOD_RESULT result;
	Channel* channel;
	float volume = -1.0f;

	result = mSystem->getChannel(channel_num, &channel);
	if(!ERRCHECK(result, __LINE__))
		return volume;

	channel->getVolume(&volume);

	return volume;
}

void AudioSystem::setVolume(int channel_num, float volume, bool relative)
{
	FMOD_RESULT result;
	Channel* channel;

	result = mSystem->getChannel(channel_num, &channel);
	if(!ERRCHECK(result, __LINE__))
		return;

	if(relative)
	{
		float curr_volume = 0.0f;
		channel->getVolume(&curr_volume);
		channel->setVolume(curr_volume + volume);
	}
	else
	{
		channel->setVolume(volume);
	}

}
