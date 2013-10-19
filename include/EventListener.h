#pragma once
#ifndef __EL
#define __EL

#include "UIManager.h"
using namespace Timewhale;

class EventListener : public Rocket::Core::EventListener

{

public:

	EventListener(Rocket::Core::Context* context, const Rocket::Core::String& value) : mContext(context), mValue(value){}


	 ~EventListener(){
		int i = 0;
	}

	/// Sends the event value through to Invader’s event processing system.

	void ProcessEvent(Rocket::Core::Event& event){
		printf("Processing event %s", event.GetType().CString());
		UIManager::get()->processEvent(event, mValue.CString());
	}


	/// Destroys the event.

	void OnDetach(Rocket::Core::Element* element){
		int i  = 0;
	}


private:

	Rocket::Core::Context* mContext;

	Rocket::Core::String mValue;

};



#endif