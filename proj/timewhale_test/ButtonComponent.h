#pragma once
#ifndef __TIMEWHALE_BUTTONCOMPONENT_H_
#define __TIMEWHALE_BUTTONCOMPONENT_H_
#include "ECS.h"
#include "SpriteComponent.h"
#include "InputManager.h"
#include "WhaleMath.h"
#include <functional>
#include <memory>

using namespace Timewhale;
class ButtonComponent : public Component {
private:
	InputManagerPtr mMan;
	Transform2DPtr transform;
	std::function<void (void)> simpleHandler;
    bool test, test2;
public:
	ButtonComponent()
		:Component(),
		simpleHandler(nullptr)
	{}

	virtual void Init() {
		mMan = InputManager::get();
		transform = GetComponent<Transform2D>();
        test = test2 = false;
	}

	virtual void Update() {
        test2 = test;
        test = mMan->getLeftMouse();

        if(!test && test2) {
		    if(transform->GetBoundsRect().ContainsPoint(mMan->getX(), mMan->getY())) {
				simpleHandler();
			}
		}
	}

	void SetSimpleHandler(const std::function<void (void)>& func) {
		simpleHandler = func;
	}
};

typedef std::shared_ptr<ButtonComponent> ButtonComponentPtr;
REGISTER_COMPONENT(ButtonComponent, "ButtonComponent");
REGISTER_DEPENDENCY(ButtonComponent, SpriteComponent);


#endif