
#include "PlatformManager.h"
#include "ProfileSample.h"
#include "InputManager.h"
#include "SettingsManager.h"
#include "TWLogger.h"
#include "UIManager.h"

#include <Windows.h>

#include <iostream>
#include <fstream>

using namespace std;
using namespace Timewhale;

PlatformManagerPtr PlatformManager::sManager = nullptr;
PlatformManagerPtr const PlatformManager::create() {
    if (!sManager) {
        sManager = PlatformManagerPtr(new PlatformManager());
        TW_SETTING(uint16_t, sManager->mWindowWidth, "windowx");
        TW_SETTING(uint16_t, sManager->mWindowHeight,"windowy");
        TW_SETTING(string, sManager->mWindowName, "windowName");
    }
    return sManager;
}

PlatformManager::PlatformManager() :
    mSDLSurface(nullptr),
    mQuit(false),
    mFullScreen(true),
    mConsole(false),
    mWindowWidth(640),
    mWindowHeight(480),
    mWindowName("TimewhaleEngine")
{}

PlatformManager::PlatformManager(const PlatformManager &that) {
    *this = that;
}

PlatformManager &PlatformManager::operator= (const PlatformManager &that) {
    if (this == &that) return *this;

    mSDLSurface = that.mSDLSurface;
    mQuit = that.mQuit;
    mFullScreen = that.mFullScreen;
    mConsole = that.mConsole;
    mWindowWidth = that.mWindowWidth;
    mWindowHeight = that.mWindowHeight;

    return *this;
}

PlatformManager::~PlatformManager() {}

bool PlatformManager::init() {

    int result = SDL_Init(SDL_INIT_EVERYTHING);
    if (result < 0) return false;

	const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
	mWindowWidth = videoInfo->current_w;
	mWindowHeight = videoInfo->current_h;

//#ifdef _DEBUG
	mSDLSurface = SDL_SetVideoMode(
		mWindowWidth, mWindowHeight, 32, SDL_FULLSCREEN);
//#else
    //mSDLSurface = SDL_SetVideoMode(
    //    mWindowWidth, mWindowHeight, 32, SDL_FULLSCREEN);
//#endif
    if (!mSDLSurface) return false;
    
    SDL_WM_SetCaption(mWindowName.c_str(), mWindowName.c_str());

    return true;
}

void PlatformManager::shutdown() {
    SDL_Quit();
    sManager.reset();
}

void PlatformManager::update() {
    PROFILE("PlatformManager - Update");
	auto input = InputManager::get();
	input->mDoubleTap = false;
    while(SDL_PollEvent(&mEvent)) {
        processMessages();
    }
	auto sMan = SettingsManager::get();
	if(input->getSaveAction()){
		sMan->saveSettings(TW_DEFAULT_SETTINGS_FILE);
	}
}

void PlatformManager::processMessages() {
	auto input = InputManager::get();
    auto ui = UIManager::get();
	auto engine = Engine::get();
	Rocket::Core::String s;  
	switch (mEvent.type){
	case SDL_KEYUP:
		input->keyUp((char)mEvent.key.keysym.sym);
        ui->uiKey((char)mEvent.key.keysym.sym, true);
		break;
	case SDL_KEYDOWN:
		input->keyDown((char)mEvent.key.keysym.sym);
        ui->uiKey((char)mEvent.key.keysym.sym, false);	
		if(mEvent.key.keysym.sym == SDLK_ESCAPE)
			UIManager::get()->tryPause();
		break;
	case SDL_MOUSEMOTION:
		input->setPosition(mEvent.motion.x, mEvent.motion.y);
        ui->uiMouseMove(mEvent.motion.x, mEvent.motion.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
		ui->uiMouseMove(mEvent.motion.x, mEvent.motion.y);
        ui->uiMouseBtn(SDL_BUTTON(mEvent.button.button)-1, false);

		if(!ui->isMouseOverUI()){
			input->setPosition(mEvent.button.x, mEvent.button.y);
			input->setMouseState(mEvent.button.button, true);
		}
		break;
	case SDL_MOUSEBUTTONUP:
		input->setPosition(mEvent.button.x, mEvent.button.y);
		input->setMouseState(mEvent.button.button, false);
        ui->uiMouseBtn(SDL_BUTTON(mEvent.button.button)-1, true);
		break;
	case SDL_RESIZABLE:
        if (mQuit) return;
        mWindowWidth = mEvent.resize.w;
        mWindowHeight = mEvent.resize.h;
		engine->resize(mWindowWidth, mWindowHeight);
		break;
	case SDL_QUIT:
        mQuit = true;
        break;
	}
	
}

void PlatformManager::setFullscreen(const bool value) {
    mFullScreen = true;
    //todo - actually make it go to fullscreen
}

const bool PlatformManager::isFullscreen() const { return mFullScreen; }
const uint16_t PlatformManager::width() const { return mWindowWidth; }
const uint16_t PlatformManager::height() const { return mWindowHeight; }
const bool PlatformManager::quitting() const { return mQuit; }

string PlatformManager::loadTextFile(const string &filename) {
    ifstream fin(filename.c_str());
	if (fin.fail()) {
		cerr << "Could not open " << filename << " for reading" << endl;
        log_sxerror("PlatformManager", "Failed to open file %s", filename);
		return "";
	}

	fin.seekg(0, ios::end);
	int count = (int)fin.tellg();
	char *data = nullptr;
	if (count > 0) {
		fin.seekg(ios::beg);
		data = new char[count+1];
		fin.read(data, count);
		data[count] = '\0';
	}
	fin.close();

    string str(data);
    if (data)
        delete [] data;
    return str;
}

void PlatformManager::setWindowTitle(const std::string &text)
{
	mWindowName = text;
	SDL_WM_SetCaption(mWindowName.c_str(), mWindowName.c_str());
}

int PlatformManager::minimizeWindow()
{
	return SDL_WM_IconifyWindow();
}

int PlatformManager::restoreWindow()
{
	SDL_SysWMinfo info;
	SDL_VERSION(&info.version);
	SDL_GetWMInfo(&info);
	ShowWindow(info.window, SW_MAXIMIZE);

	return 0;
}