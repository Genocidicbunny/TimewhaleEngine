/*
 * WhaleTimer class
 * Welcome to the WhaleTimer!. WhaleTimer is a managed object and can only be created
 * through the createTimer static method. Timers are updated by the engine each tick
 * and timers with alarms set can trigger callbacks.
*/
#pragma once
#ifndef __WHALE_TIMER
#define __WHALE_TIMER

#include "TimewhaleApi.h"
#include <memory>
#include <unordered_map>
#include <functional>
#include <string>

namespace Timewhale {

    TIMEWHALEAPI enum  {
        //Timers will reset after firing. Otherwise the timer
        //will only fire once.
        TW_TIMER_ALARM_REPEAT = 0x01,

        //The timer alarm will be set to deactive, requiring the
        //user to manually activate it.
        TW_TIMER_ALARM_DEACTIVE = 0x02,
    };

    class TIMEWHALEAPI WhaleTimer {
        friend class TimewhaleEngine;
        typedef std::unordered_map<int, std::shared_ptr<WhaleTimer>> TimerMap;

        static TimerMap sTimers;                        //Map of all timers.
        static std::shared_ptr<WhaleTimer> sEngineTimer;//Engine's timer.
        float mTimepoint;                               //Delta times are reference from this point.
        uint16_t mTimerOptions;                         //Timer options.
        float mAlarmTime;                               //Time in seconds to trigger alarm.
        bool mAlarmActive;                              //True if the alarm is active. 
        std::function<void (void)>  mAlarmCallback;     //Callback function for triggered alarms.

        /* init
         * This method is called by the TimewhaleEngine to initialize the
         * timer system.        */
        static bool init();

        /* shutdown
         * This method is called by the TimewhaleEngine to shutdown the
         * timer system and clean up any loose timers. All timer alarms
         * will be deactivated after this is called     */
        static void shutdown();

        /* updateTimers
         * This method is called by the TimewhaleEngine to update the timers
         * and check for triggered alarms.*/
        static void updateTimers();

        /* checkAlarm
         * This method checks to see if the alarm time has elapsed. It returns
         * true if it has otherwise false.
         *\return   bool        True if the alarm has triggered.        */
        bool checkAlarm();

        /* triggerAlarm
         * This method is called by the TimewhaleEngine to trigger alarm
         * callbacks.*/
        void triggerAlarm();

        /* constructors
         * Constructors are private so you cannot instantiate timers directly!
         * Use the createTimer method!*/
        WhaleTimer();
        WhaleTimer(const WhaleTimer &);
        WhaleTimer &operator= (const WhaleTimer &);
    public:
        /* createTimer
         * This method creates and returns a newly made timer. If the timer with
         * the given name already exists then it returns that one.
         *\param    string          Name of the new timer.
         *\return   WhaleTimerPtr   Shared ptr to a the timer.*/
        static std::shared_ptr<WhaleTimer> createTimer(const std::string &name);
        
        /* getTimer
         * This method returns a timer incase you lost your reference. You have to
         * pass in a WhaleTimerPtr to be set to the retrieved timer. If the timer
         * doesn't exist however the method will return false! Besure to check.
         *\param    string          Name of the timer.
         *\param    &WhaleTimerPtr  A pointer to set to the retrieved timer.
         *\return   bool            True if the timer exists, otherwise false.  */
        static bool getTimer(const std::string &name, std::shared_ptr<WhaleTimer> &timer);

        /* getCurrentTime
         * This method gets the current system time in seconds.
         *\return   float           Current system time in seconds.     */
        static float getCurrentTime();
        
        /* getEngineDelta
         * This method returns the current delta from the engine's timer.
         *\return   float           Current engine delta in seconds.    */
        static float getEngineDelta();

        /* setTime
         * This method sets the time point of the timer to the current time.
         * Delta time is measured from the recorded timepoint.  */
        void setTime();

        /* getDeltaTime
         * This method returns delta time of the timer. Delta time is measured
         * from either timer creation or the last call to setTime.  */
        float getDeltaTime();
        
        /* setAlarm
         * This method sets an alarm for the timer. Alarms are given a time in
         * seconds and a callback function (use lambdas!). Optionally you can
         * provide some settings as seen above. Setting an alram will make a
         * call to setTime() unless the TW_TIMER_ALARM_DEACTIVE option is set.
         *\param    float           Time in seconds till the alarm is triggered.
         *\param    void function   Callback function to be called.
         *\param    uint16_t        Alarm options (Optional).       */
        void setAlarm(float seconds, const std::function<void (void)>&, uint16_t options = 0);       
        
        /* activateAlarm
         * This method activates an alarm if it is current deactive. Calling this
         * method will make a call to setTime. */
        void activateAlarm();

        /* deactivateAlarm
         * This method will deactivate an active alarm. */
        void deactivateAlarm();

        /* resetAlarm
         * This method will reset the alarm if it has been previously set.
         * Nothing will happen on timers that have not had an alarm set.
         *\param    float           The number of seconds to set the alarm.
                                    If no number is given, or anything <= 0,
                                    the previous time will be used.
         *\param    uint16_t        Alarm options, if you want to make changes. */
        void resetAlarm(float seconds = 0.0f, uint16_t options = 0);
        
        ~WhaleTimer();
    };

    typedef std::shared_ptr<WhaleTimer> WhaleTimerPtr;
}


//TIMEWHALE_TEMPLATE template class TIMEWHALEAPI std::function<void (void)>;
//TIMEWHALE_TEMPLATE template class TIMEWHALEAPI std::unordered_map<int, std::shared_ptr<WhaleTimer>>;

#endif
