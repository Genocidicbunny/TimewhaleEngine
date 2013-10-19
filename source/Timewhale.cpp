
#include <chrono>
#include "Timewhale.h"

#include "TWLogger.h"
#include "ProfileSample.h"
#include "ProfileLogHandler.h"

#include "ResourceManager.h"
#include "SettingsManager.h"
#include "PlatformManager.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "UIManager.h"
#include "WhaleTimer.h"
#include "Engine.h"

using namespace Timewhale;

bool TimewhaleEngine::running = false;

bool TimewhaleEngine::Initialize(vector<string> init_packs) {
    
	bool result;
    
    // Give the profiler an output handler.
    ProfileSample::sOutputHandler = ProfileLogHandlerPtr(new ProfileLogHandler());
    
    // Create the managers.
    auto settings = SettingsManager::create();
    auto platform = PlatformManager::create();
    auto input = InputManager::create();
	auto resources = ResourceManager::create();
    auto engine = Engine::create();
    auto sceneManager = SceneManager::Create();
    auto ui = UIManager::create();

    // Load the settings file.
    settings->loadSettings(TW_DEFAULT_SETTINGS_FILE);

	// Initialize the platform manager.
    result = platform->init();
    if (!result) {
        log_sxerror("TimewhaleEngine", "Platform Manager has failed to initialize!");
        return false;
    }

    // Initialize the whale timer.
    WhaleTimer::init();

    // Initialize the engine.
    result = engine->init(init_packs);
    if (!result) {
        log_sxerror("TimewhaleEngine", "Engine has failed to initialize!");
        return false;
    }

    result = sceneManager->Init();
    if (!result) {
        log_sxerror("TimewhaleEngine", "SceneManager has failed to initialize!");
        return false;
    }

    result = ui->init();
    if (!result) {
        log_sxerror("TimewhaleEngine", "UIManager has failed to initialize!");
        return false;
    }

    return true;
}

void TimewhaleEngine::Run() {
    running = true;

    //float begin_frame_time ;
    auto platform = PlatformManager::get();
    auto engine = Engine::get();

    engine->run();

    bool gametick = true;
    auto gameTimer = WhaleTimer::createTimer("gameTimer");
    gameTimer->setAlarm(1.0f/60.0f, [&gametick](void) { gametick = true; }, TW_TIMER_ALARM_REPEAT);

    while (!platform->quitting() && running) {
		
        platform->update();
        WhaleTimer::updateTimers();
        UIManager::get()->update();
        if (gametick) {
            SceneManager::get()->Update();
            gametick = false;
        }
	
        engine->update();

    }
	
}

void TimewhaleEngine::Pause() {
    auto engine = Engine::get();

    engine->pause();
}

void TimewhaleEngine::Quit() {
    running = false;
}

void TimewhaleEngine::Shutdown() {
	//Force network shutdown here because it may block
	NetworkSystem::get()->Shutdown();
    SettingsManager::get()->saveSettings();
	SceneManager::get()->Shutdown();
    Engine::get()->shutdown();
    WhaleTimer::shutdown();
    PlatformManager::get()->shutdown();
    SettingsManager::get()->shutdown();

    ProfileSample::output();
    ProfileSample::sOutputHandler.reset();
}