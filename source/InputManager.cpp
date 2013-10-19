
#include "InputManager.h"
#include "ProfileSample.h"
#include "TWLogger.h"
#include "SettingsManager.h"

using namespace std;
using namespace Timewhale;

InputManagerPtr InputManager::iManager = nullptr;

InputManagerPtr const InputManager::create() {
    if (!iManager) {
        iManager = InputManagerPtr(new InputManager());
    
        TW_SETTING(char, iManager->myMacros[FORWARD], "Forward");
        TW_SETTING(char, iManager->myMacros[BACKWARD],"Backward");
	    TW_SETTING(char, iManager->myMacros[LEFT],"Left");
	    TW_SETTING(char, iManager->myMacros[RIGHT],"Right");
		TW_SETTING(char, iManager->myMacros[SAVE],"Save");
		TW_SETTING(char, iManager->myMacros[BASE],"Base");
		TW_SETTING(char, iManager->myMacros[INFLUENCE],"Influence");
		TW_SETTING(char, iManager->myMacros[GENERATOR],"Generator");
		TW_SETTING(char, iManager->myMacros[OVERLOAD],"Overload");
		TW_SETTING(char, iManager->myMacros[CHARGE],"Charge");
		TW_SETTING(char, iManager->myMacros[ZOOM], "Zoom");
		TW_SETTING(char, iManager->myMacros[CANCEL], "Cancel Build");
	    TW_SETTING(Uint32, iManager->betweenClicks,"DoubleClickTime");
    }
    return iManager;
}

InputManager::InputManager(){
	controlKeyCount = TIMEWHALE_INPUT_ARRAY_SIZE;
	betweenClicks = 500;
	mScrollDown = false;
	mScrollUp = false;
	mTime = 0.0;
	mRight = false;
	mLeft = false;
	mX = 0;
	mY = 0;
	mMouseLeft = false;
	mMouseRight = false;
	mDoubleTap = false;
	mIsKeyboardAction = false;
	//mTime = SDL_GetTicks();
	myMacros[FORWARD] = (char)SDLK_w;
	myMacros[BACKWARD] = (char)SDLK_s;
	myMacros[LEFT] = (char)SDLK_a;
	myMacros[RIGHT] = (char)SDLK_d;
	myMacros[SAVE] = (char)SDLK_p;
	myMacros[BASE] = (char)SDLK_b;
	myMacros[INFLUENCE] = (char)SDLK_i;
	myMacros[GENERATOR] = (char)SDLK_g;
	myMacros[OVERLOAD] = (char)SDLK_o;
	myMacros[CHARGE] = (char)SDLK_c;
	myMacros[CANCEL] = (char)SDLK_BACKSPACE;
	myActions[FORWARD] = false;
	myActions[BACKWARD] = false;
	myActions[LEFT] = false;
	myActions[RIGHT] = false;
	myActions[SAVE] = false;
	myActions[BASE] = false;
	myActions[INFLUENCE] = false;
	myActions[GENERATOR] = false;
	myActions[OVERLOAD] = false;
	myActions[CHARGE] = false; 
	myActions[CANCEL] = false;
}

InputManager::InputManager(const InputManager &that) {
    *this = that;
}

InputManager::~InputManager() {}

InputManager &InputManager::operator= (const InputManager &that) {
    if (this == &that) return *this;

    return *this;
}

void InputManager::update() {

}

void InputManager::keyUp(char keyType){
	mKeyUp = true;
	mKeyDown = false;
	checkKey(keyType);
}

void InputManager::keyDown(char keyType){
	mKeyUp = false;
	mKeyDown = true;
	checkKey(keyType);
}

void InputManager::checkKey(char keyType){
	for(int i = 0; i < TIMEWHALE_INPUT_ARRAY_SIZE; i++){
		if(keyType == myMacros[i]){
			if(mKeyUp){
				//myActions[i] = false;
				//mIsKeyboardAction = false;
				//Switch is for Debug
				switch(i){
				case FORWARD:
					log_sxinfo("InputManager", "Forward Released");
					break;
				case BACKWARD:
					log_sxinfo("InputManager", "Backward Released");
					break;
				case LEFT:
					log_sxinfo("InputManager", "Left Released");
					break;
				case RIGHT:
					log_sxinfo("InputManager", "Right Released");
					break;
				case SAVE:
					log_sxinfo("InputManager", "Save Released");
					break;
				case BASE:
					break;
				case INFLUENCE:
					break;
				case GENERATOR:
					break;
				case OVERLOAD:
					break;
				case CHARGE:
					break;
				}
			}else if(mKeyDown){
				mIsKeyboardAction = true;
				myActions[i] = true;
				for(int j = 0; j < TIMEWHALE_INPUT_ARRAY_SIZE; j++){
					if(i != j){
						myActions[j] = false;
					}
				}
				//mIsKeyboardAction = true;
				//Switch for debug
				switch(i){
				case FORWARD:
					log_sxinfo("InputManager", "Forward Pressed");
					break;
				case BACKWARD:
					log_sxinfo("InputManager", "Backward Pressed");
					break;
				case LEFT:
					log_sxinfo("InputManager", "Left Pressed");
					break;
				case RIGHT:
					log_sxinfo("InputManager", "Right Pressed");
					break;
				case SAVE:
					log_sxinfo("InputManager", "Save Pressed");
					break;
				case BASE:
					break;
				case INFLUENCE:
					break;
				case GENERATOR:
					break;
				case OVERLOAD:
					break;
				case CHARGE:
					break;
				}
			}
		}
	}
}

void InputManager::setPosition(int X, int Y){
	mX = X;
	mY = Y;
}

void InputManager::setMouseState(Uint8 state, bool mousePressed){
	if(state != NULL){
		if(state == SDL_BUTTON_WHEELDOWN){
			mScrollDown = true;
		}else if(state == SDL_BUTTON_WHEELUP){
			mScrollUp = true;
		}else if(state == SDL_BUTTON_LEFT){
			mMouseLeft = mousePressed; log_sxinfo("InputManager", "LeftClick Pressed");
			if(mousePressed){
				float test = SDL_GetTicks();
				if(mTime > SDL_GetTicks() - betweenClicks){
					mDoubleTap = true; log_sxinfo("InputManager", "DoubleTap");
				}else{
					mDoubleTap = false;
				}
				if(mDoubleTap && !mousePressed){
					mDoubleTap = false;log_sxinfo("InputManager", "DoubleTap Released");
				}
				mTime = SDL_GetTicks();
			}
		}else if(state == SDL_BUTTON_RIGHT){
				mMouseRight = mousePressed; log_sxinfo("InputManager", "RightClick Pressed");
		}else{
			mMouseRight = false;
			mMouseLeft = false;
		}
	}
}

void InputManager::clearInput(){
	mIsKeyboardAction = false;
	mScrollDown = false;
	mScrollUp = false;
	//mMouseLeft = false;
	//mMouseRight = false;
	for(int i = 0; i < TIMEWHALE_INPUT_ARRAY_SIZE; i++){
		myActions[i] = false;
	}
}

int InputManager::returnKeyState(){
	for(int i = START_OF_ABILITIES; i < TIMEWHALE_INPUT_ARRAY_SIZE; i++){
		if(myActions[i] == true){
			switch(i){
			case BASE:
				return 0;
				break;
			case INFLUENCE:
				return 1;
				break;
			case GENERATOR:
				return 2;
				break;
			case OVERLOAD:
				return 3;
				break;
			case CHARGE:
				return 4;
				break;
			}
		}
	}
}

//Positional Returns
float InputManager::getX(){return mX;}
float InputManager::getY(){return mY;}
bool InputManager::getDoubleTap(){return mDoubleTap;}

//Mouse left and right Button returns
bool InputManager::getLeftMouse(){return mMouseLeft;}
bool InputManager::getRightMouse(){return mMouseRight;}

bool InputManager::getScrollUp(){return mScrollUp;}
bool InputManager::getScrollDown(){return mScrollDown;}

//keyboard state returns
bool InputManager::getKeyUp(){return mKeyUp;} 
bool InputManager::getKeyDown(){return mKeyDown;}
bool InputManager::getForwardAction(){return myActions[FORWARD];}
bool InputManager::getBackwardAction(){return myActions[BACKWARD];}
bool InputManager::getLeftAction(){return myActions[LEFT];}
bool InputManager::getRightAction(){return myActions[RIGHT];}
bool InputManager::getSaveAction(){return myActions[SAVE];}
bool InputManager::getGeneratorAction(){return myActions[GENERATOR];}
bool InputManager::getInfluenceAction(){return myActions[INFLUENCE];}
bool InputManager::getBaseAction(){return myActions[BASE];}
bool InputManager::getOverloadAction(){return myActions[OVERLOAD];}
bool InputManager::getChargeAction(){return myActions[CHARGE];}
bool InputManager::getCancelAction(){return myActions[CANCEL];}

bool InputManager::getActions(){return myActions;}

bool InputManager::isKeyboardAction(){return mIsKeyboardAction;}