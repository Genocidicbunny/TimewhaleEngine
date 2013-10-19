/*
 * Input Manager class
 * The input manager abstracts and handles everything that has to do with
 * the input, such as pressing buttons and clicking on the screen.
 * This manager is using SDL.

 * The input manager is a manager! Which means it is singleton and can be
 * referenced from anywhere. Include this header and get the manager with the
 * get method.
 *
 * Example - 
 * 
 * #include "InputManager.h" 
 * ...
 * auto platform = InputManager::get();
 * ...
*/

#pragma once
#ifndef __INPUT_MANAGER
#define __INPUT_MANAGER

#include "TimewhaleApi.h"
#include "SDL\SDL.h"

#include <memory>

#define TIMEWHALE_INPUT_ARRAY_SIZE 12
//The first four are for camera not abilites
#define START_OF_ABILITIES 4

namespace Timewhale {

    class InputManager {
		//This is the singleton object for the manager.
        static std::shared_ptr<InputManager> iManager;

		/* Constructors */
        InputManager();
        InputManager(const InputManager &);
        InputManager &operator= (const InputManager &);
		friend class PlatformManager;

    public:
		/* get
		 * This method returns the pointer to the manager. */
        inline static std::shared_ptr<InputManager> const &get() {
            return iManager;
        }

        /* create
         * This method creates the manager object. */
        static std::shared_ptr<InputManager> const create();

        ~InputManager();
		/* update
		 * This method updates the manager, and processes the OS messages.
		 * It is important to call this every game tick to respond to the
		 * messages.	*/
		void update();

		//Getters for X and Y
		void clearInput();

		float getX();
		float getY();
		//Getters for left and right mouse pressed and doubleTap
		bool getLeftMouse();
		bool getRightMouse();
		bool getDoubleTap();
		bool getScrollUp();
		bool getScrollDown();
		//Getters for movement and actions in game.
		bool getKeyUp(); 
		bool getKeyDown();
		bool getForwardAction();
		bool getBackwardAction();
		bool getLeftAction();
		bool getRightAction();
		bool getSaveAction();
		bool getGeneratorAction();
		bool getInfluenceAction();
		bool getBaseAction();
		bool getOverloadAction();
		bool getChargeAction();
		bool getCancelAction();

		bool getActions();

		bool isKeyboardAction();
		int returnKeyState();

	private:
		float mTime;
		Uint32 betweenClicks;
		//keeps track of the macros
		char myMacros[TIMEWHALE_INPUT_ARRAY_SIZE];
		//keeps track of the actions tied to the macros
		bool myActions[TIMEWHALE_INPUT_ARRAY_SIZE];
		int controlKeyCount;

		void keyUp(char keyType);
		void keyDown(char KeyType);
		void checkKey(char keyType);
		void setPosition(int X, int Y);
		void setMouseState(Uint8 state, bool mousePressed);

		bool mKeyUp;
		bool mKeyDown;
		bool mForward;
		bool mBackward;
		bool mLeft;
		bool mRight;
		bool mScrollUp;
		bool mScrollDown;
		bool mIsKeyboardAction;

		//bool mMouseUp;
		//bool mMouseDown;
		bool mMouseLeft;
		bool mMouseRight;
		bool mDoubleTap;

		float mX;
		float mY;

		enum directions {FORWARD, BACKWARD, LEFT, RIGHT, SAVE, BASE, INFLUENCE, GENERATOR, OVERLOAD, CHARGE, ZOOM, CANCEL};
    };

    typedef std::shared_ptr<InputManager> InputManagerPtr;
}

#endif