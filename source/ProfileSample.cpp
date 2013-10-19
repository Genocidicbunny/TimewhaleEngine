
#include "ProfileSample.h"
#include <assert.h>

using namespace std;
using namespace Timewhale;

int ProfileSample::sLastOpenedSample = -1;
int ProfileSample::sOpenSampleCount = 0;
int ProfileSample::sNumSamples = 0;
float ProfileSample::sRootBegin = -1.0f;
float ProfileSample::sRootEnd = -1.0f;
IProfilerOutputHandlerPtr ProfileSample::sOutputHandler = nullptr;
ProfileSample::_profileSample ProfileSample::samples[MAX_PROFILER_SAMPLES];

ProfileSample::ProfileSample(const string &sampleName) {
    int i = 0;
    int storeIndex = -1;
    sNumSamples++;

    for (i = 0; (i < sNumSamples && i < MAX_PROFILER_SAMPLES); ++i) {
        if (!samples[i].isValid) {
            if(storeIndex < 0) storeIndex = i;
        } else {
            if (samples[i].name == sampleName) {
                assert(!samples[i].isOpen && "Tried to profile a sample which was already being profiled");
                mSampleIndex = i;
                mParentIndex = sLastOpenedSample;
                sLastOpenedSample = i;
                samples[i].parentCount = sOpenSampleCount;
                ++sOpenSampleCount;
                samples[i].isOpen = true;
                ++samples[i].callCount;
                samples[i].startTime = getTime();
                
                if (mParentIndex < 0)
                    sRootBegin = samples[i].startTime;
                return;
            }
        }
    }

    assert(storeIndex >= 0 && "Profiler has run out of sample slots!");
    samples[storeIndex].isValid = true;
    samples[storeIndex].name = sampleName;
    mSampleIndex = storeIndex;
    mParentIndex = sLastOpenedSample;
    sLastOpenedSample = storeIndex;
    samples[i].parentCount = sOpenSampleCount;
    sOpenSampleCount++;
    samples[storeIndex].isOpen = true;
    samples[storeIndex].callCount = 1;

    samples[storeIndex].totalTime = 0.0f;
    samples[storeIndex].childTime = 0.0f;
    samples[storeIndex].startTime = getTime();
    if (mParentIndex < 0)
        sRootBegin = samples[storeIndex].startTime;
}

ProfileSample::~ProfileSample() {
    float endTime = getTime();
    samples[mSampleIndex].isOpen = false;

    float timeTaken = endTime - samples[mSampleIndex].startTime;

    if (mParentIndex >= 0)
        samples[mParentIndex].childTime += timeTaken;
    else
        sRootEnd = endTime;

    samples[mSampleIndex].totalTime += timeTaken;
    sLastOpenedSample = mParentIndex;
    --sOpenSampleCount;
}

void ProfileSample::output() {
    assert(sOutputHandler && "Profiler has no output handler set.");

    sOutputHandler->BeginOutput();

    for(int i = 0; i < MAX_PROFILER_SAMPLES; ++i) {
        
        if (!samples[i].isValid) continue;

        float sampleTime, percentage;
        sampleTime = samples[i].totalTime - samples[i].childTime;
		//assert(sRootEnd - sRootBegin != 0.0f); //Assertion checked for by below conditional:
		(sRootEnd - sRootBegin != 0.0f ) ?
			percentage = (sampleTime / (sRootEnd - sRootBegin)) * 100.0f :
			percentage = 0.0f;

        float totalPc;
        totalPc = samples[i].averagePc * samples[i].dataCount;
        totalPc += percentage;
        samples[i].dataCount++;
		assert(samples[i].dataCount != 0);
        samples[i].averagePc = totalPc / samples[i].dataCount;

        if ((samples[i].minPc == -1) || (percentage < samples[i].minPc))
            samples[i].minPc = percentage;
        if ((samples[i].maxPc == -1) || (percentage > samples[i].maxPc))
            samples[i].maxPc = percentage;

        sOutputHandler->Sample(
            samples[i].minPc,
            samples[i].averagePc,
            samples[i].maxPc,
            samples[i].callCount,
            samples[i].name,
            samples[i].parentCount);

        samples[i].callCount = 0;
        samples[i].totalTime = 0.0f;
        samples[i].childTime = 0.0f;
    }

    sOutputHandler->EndOutput();
}

