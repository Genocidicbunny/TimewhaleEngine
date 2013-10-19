#include "WhaleTimer.h"
#include "SDL\SDL.h"

using namespace std;
using namespace Timewhale;

WhaleTimer::TimerMap WhaleTimer::sTimers;
WhaleTimerPtr WhaleTimer::sEngineTimer;

WhaleTimer::WhaleTimer() :
    mAlarmTime(0.0f),
    mAlarmActive(false),
    mTimerOptions(0)
{
    setTime();
}

WhaleTimer::WhaleTimer(const WhaleTimer &that) {
    *this = that;
}

WhaleTimer &WhaleTimer::operator= (const WhaleTimer &that) {
    if (this == &that) return *this;
    mTimepoint = that.mTimepoint;
    mTimerOptions = that.mTimerOptions;
    mAlarmTime = that.mAlarmTime;
    mAlarmActive = that.mAlarmActive;
    mAlarmCallback = that.mAlarmCallback;
    return *this;
}

WhaleTimer::~WhaleTimer() {
    mAlarmActive = false;
}

bool WhaleTimer::init() {
    //todo some how check that sdl has been initialized.
    sEngineTimer = WhaleTimerPtr(new WhaleTimer());
    return true;
}

void WhaleTimer::shutdown() {
    sTimers.clear();
    sEngineTimer.reset();
}

void WhaleTimer::updateTimers() {
    if (sEngineTimer->checkAlarm())
        sEngineTimer->triggerAlarm();

    auto itor = sTimers.begin();
    auto end = sTimers.end();

    for (; itor != end; ++itor) {
        if (itor->second->checkAlarm())
            itor->second->triggerAlarm();
    }
}

bool WhaleTimer::checkAlarm() {
    if (!mAlarmActive) return false;

    return (getDeltaTime() >= mAlarmTime);
}

void WhaleTimer::triggerAlarm() {
    if (!mAlarmCallback) return;

    mAlarmCallback();

    if (!(mTimerOptions | TW_TIMER_ALARM_REPEAT))
        mAlarmActive = false;
    else
        setTime();
}

WhaleTimerPtr WhaleTimer::createTimer(const std::string &name) {
    hash<string> strHash;
    int id = strHash(name);

    if (sTimers.count(id) > 0)
        return sTimers.at(id);
    
    auto newTimer = WhaleTimerPtr(new WhaleTimer());
    sTimers.insert(make_pair(id, newTimer));
    return newTimer;
}

bool WhaleTimer::getTimer(const string &name, WhaleTimerPtr &timer) {
    hash<string> strHash;
    int id = strHash(name);

    if (sTimers.count(id) > 0) {
        timer = sTimers.at(id);
        return true;
    }
    return false;
}

float WhaleTimer::getCurrentTime() {
    return ((float)SDL_GetTicks() / 1000.0f);
}

float WhaleTimer::getEngineDelta() {
    return sEngineTimer->getDeltaTime();
}

void WhaleTimer::setTime() {
    mTimepoint = getCurrentTime();
}

float WhaleTimer::getDeltaTime() {
    float curTime = getCurrentTime();
    return curTime - mTimepoint;
}

void WhaleTimer::setAlarm(
    float seconds, 
    const std::function<void (void)> &func, 
    uint16_t options) 
{
    mAlarmTime = seconds;
    mAlarmCallback = func;   
    mTimerOptions = options;  

    if (mAlarmTime <= 0.0f) return;
    if (mTimerOptions & TW_TIMER_ALARM_DEACTIVE) return;
    
    mAlarmActive = true;
    setTime();
}

void WhaleTimer::activateAlarm() {
    mAlarmActive = true;
    setTime();
}

void WhaleTimer::deactivateAlarm() {
    mAlarmActive = false;
}

void WhaleTimer::resetAlarm(float seconds, uint16_t options) {
    if (seconds > 0.0f)
        mAlarmTime = seconds;

    if (options != 0)
        mTimerOptions = options;

    if (mAlarmTime <= 0.0f) return;
    if (mTimerOptions & TW_TIMER_ALARM_DEACTIVE) return;
    
    mAlarmActive = true;
    setTime();   
}

