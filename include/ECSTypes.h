#pragma once
#ifndef __TIMEWHALE_ECSTYPES_H_
#define __TIMEWHALE_ECSTYPES_H_

#include <memory>
#include "IComponent.h"
//#include "ECS\Component.h"
#include "Entity.h"
//#include "ECS\EntityManager.h"
//#include "ECS\ComponentManager.h"


//		interface IComponent
		typedef std::shared_ptr<Timewhale::IComponent>			IComponentPtr;
		typedef IComponentPtr&										IComponentPtrRef;

//		class Component;
		//typedef std::shared_ptr<Timewhale::ECS::Component>			ComponentPtr;
		//typedef ComponentPtr&										ComponentPtrRef;
//		class Entity;
		typedef std::shared_ptr<Timewhale::Entity>				EntityPtr;
		typedef EntityPtr&											EntityPtrRef;
//		class EntityManager;
//		typedef std::shared_ptr<Timewhale::ECS::EntityManager>		EntityManagerPtr;
//		typedef EntityManagerPtr&									EntityManagerPtrRef;
////		class ComponentManager;
//		typedef std::shared_ptr<Timewhale::ECS::ComponentManager>	ComponentManagerPtr;
//		typedef ComponentManagerPtr&								ComponentManagerPtrRef;

//}
//}


#endif