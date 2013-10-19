/*
 * Player Input Component
 * This is example of how to make a simple component. It looks easy huh!
 * Well it is easy! But it is important to remember to add that registration
 * bit at the end. Just use the usual copy pasta routines on it.
*/
#pragma once
#ifndef __PLAYER_INPUT_COMPONENT
#define __PLAYER_INPUT_COMPONENT

/* ECS is the only header that must be included.
 * These others are totally optional and up to the needs
 * of your component. This component will move the entity
 * it is attached to, to the position of your mouse. So we
 * will need to reference the entities transform2d component.
 * We will also be playing a sound whenever the user clicks
 * so I've included the basic audio resource header. And 
 * finally we get user input from the input manager. */
#include "ECS.h"	
#include "TransformComponent.h"
#include "BasicAudioResource.h"
#include "InputManager.h"
#include "SceneManager.h"
#include "Camera.h"
#include "AudioSystem.h"

using namespace Timewhale;

/* Your component should inherit from the Component class. This will
 * give you all the necessary functionality for creating your components. */
class PlayerInputComp : public Component {
    InputManagerPtr input;
    Transform2DPtr transform;
    Camera* camera;

public:

    virtual void Init() {
        input = InputManager::get();
        transform = GetComponent<Transform2D>();
        
		camera = SceneManager::GetCurrentScene()->GetCamera();
    }

	/* This component is super simple and only has an update. Your 
	 * components can have additional data and methods however to
	 * fit your needs. */
	virtual void Update() {

		/* We use the transform component's set position to
		 * change the position of the entity. */
		transform->SetPosition(input->getX(), input->getY());

		/* Here we check to see if the left mouse button has
		 * been pressed. */
		if (input->getLeftMouse()) {
			/* If it has, then we play the "boo" sound effect.
			 * You can play audio resources like this. */
			BasicAudio("boo").Play(input->getX(), input->getY());
		}

        float x = 0.0f, y = 0.0f;
        if (input->getForwardAction())
            y = -0.2f;
        if (input->getBackwardAction())
            y = 0.2f;
        if (input->getLeftAction())
            //AudioSystem::setDfactor(0.9f);
            x = -0.2f;
        if (input->getRightAction())
            x = 0.2f;
            //AudioSystem::setDfactor(10.0f);
        camera->Move(x, y);

		/* Note, in this simple example I don't care about efficiency.
		 * But in a real game we should probably store the component
		 * returned by GetComponent and the BasicAudio so we don't have
		 * to constantly look them up every update! */
	}
};

/* This stuff registers the component with the ECS system. If you don't do this
 * Then  you won't beable to make components! Just copy and paste this for the
 * most part and change the names to be relevent to your component. */
REGISTER_COMPONENT_WFLAGS(PlayerInputComp, "PlayerInputComp", CREG_FLAGS_USERCOMP);
static const TypeID PlayerInputCompTypeID(ctid<PlayerInputComp>());

/* This typedef is just to make life a little easier. I suggest making one for
 * your components as well. */
typedef std::shared_ptr<PlayerInputComp> PlayerInputCompPtr;
#endif
