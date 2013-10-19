
#include "Engine.h"
#include "PlatformManager.h"
#include "TWLogger.h"
#include "SDL\SDL_syswm.h"
#include "UIManager.h"
#include "SpriteComponent.h"
#include "TransformComponent.h"
#include "SceneTable.h"
#include "BasicAudioResource.h"

using namespace std;
using namespace Timewhale;

EnginePtr Engine::sEngine = nullptr;

EnginePtr const Engine::create() {
    if (!sEngine)
        sEngine = EnginePtr(new Engine());
    return sEngine;
}

Engine::Engine() {
    mAudio = AudioSystem::create();
    mRendering = RenderSystem::create();
	mNetwork = NetworkSystem::create();
}

Engine::Engine(const Engine &that) {
    *this = that;
}

Engine &Engine::operator= (const Engine &that) {
    if (this == &that) return *this;
    return *this;
}

Engine::~Engine() {}

bool Engine::init(std::vector<string> resource_system_init_packs) {
    
    if(!mAudio->init()) {
        log_sxerror("TimewhaleEngine", "AudioSystem failed to initialize!");
        return false;
    }

	// Initialize rendering
	auto platform = PlatformManager::get();
	SDL_SysWMinfo SysInfo;
	SDL_VERSION(&SysInfo.version); //Set SDL version
  
	if(SDL_GetWMInfo(&SysInfo) <= 0) {
		printf("%s : %d\n", SDL_GetError(), SysInfo.window);
		return false;
	}
  
	HWND WindowHandle = SysInfo.window; // Get the window handle
	//DEBUG_ONLY(ShowWindow(WindowHandle, 3)); //maximize window
	if(!mRendering->Initialize(platform->width(), platform->height(), WindowHandle)) {
		log_sxerror("TimewhaleEngine", "RenderSystem failed to initialize!");
        return false;
	}

	if(!mResources.Initialize(resource_system_init_packs)) {
		log_sxerror("TimewhaleEngine", "Resource System failed to initialize!");
		return false;
	}
	
	if(!mNetwork->Initialize()) {
		log_sxerror("TimewhaleEngine", "NetworkSystem failed to initialize!");
		return false;
	}

    return true;
};

void Engine::run() {
    mPaused = false;


}

void Engine::update() {
    if (!mPaused) {

		mNetwork->Update();
        mAudio->update();
		mRendering->Draw();
    }
}

void Engine::pause() {
    mPaused = true;
}

void Engine::shutdown() {
	mNetwork->Shutdown();
    mAudio->shutdown();
	mRendering->Shutdown();
	mResources.Shutdown();
    sEngine.reset();
}

void Engine::resize(int windowWidth, int windowHeight) {
	mRendering->Resize(windowWidth, windowHeight);
}