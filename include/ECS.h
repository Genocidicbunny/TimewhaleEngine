#pragma once
/*! This is the primary include file for ECS
 * All of the ECS headers are included in this one
*/


#ifndef __TIMEWHALE_ECS_H_
#define __TIMEWHALE_ECS_H_

#include <memory>
#include <cassert>
#include <unordered_map>
#include <unordered_set>
#include <map>
#include <thread>
#include <string>
#include <type_traits>

#include "cppBtree\btree_set.h"
#include "cppBtree\btree_map.h"
#include "IComponent.h"
#include "ECSTypes.h"
#include "ComponentRegistry.h"
#include "TWMemory.h"
#include "TWLogger.h"

#include "Event.h"

namespace Timewhale {
	class Scene;
	class Engine; //temp
	class Component;
	class Camera;
	using namespace std;
	
	//Component tag group structure

	typedef std::unordered_set<InstanceID> _componentTagSet;
	typedef _componentTagSet::iterator _cTSetItor;
	struct _componentTagContainer {
		_componentTagSet mComponentSet;
		TypeID m_setID;


		_componentTagContainer(const TypeID setID) : m_setID(setID) {
		}
		_componentTagContainer() :m_setID(0) {}

		bool add(const InstanceID id) {
			std::pair<_cTSetItor, bool> result = mComponentSet.insert(id);
			return result.second;
		}
		bool remove(const InstanceID id) {
			_cTSetItor result = mComponentSet.find(id);
			if(result != mComponentSet.end()) {
				mComponentSet.erase(result);
				return true;
			}
			return false;
		}
		const bool has(const InstanceID id) const {
			return (mComponentSet.find(id) != mComponentSet.end());
		}
		void make_empty() {
			mComponentSet.clear();
		}
		bool operator==(const _componentTagContainer& other) {
			return (m_setID == other.m_setID);
		}
		bool operator!=(const _componentTagContainer& other) {
			return (m_setID != other.m_setID);
		}
		const size_t operator()() const{
			static std::hash<TypeID> mHasher;
			return mHasher(m_setID);
		}
	};
	//struct _componentTagContainerHasher {
	//	static const std::hash<std::wstring> _ctcHasher;

	//};
	typedef std::unordered_map<TypeID, _componentTagContainer> ComponentTagGroups;
	typedef ComponentTagGroups::iterator CTGroupsItor;
	typedef ComponentTagGroups::const_iterator CTGroupsCItor;
	typedef ComponentTagGroups::reverse_iterator CTGroupsRItor;

	//Entity ComponentID set

	typedef std::unordered_set<InstanceID> _entityComponentSet;
	typedef _entityComponentSet::iterator _eCSetItor;
	struct _entityContainer {
		_entityComponentSet mComponentSet;
		ComponentTagGroups mTagGroups;
		InstanceID mEntityID;
		

		_entityContainer(const InstanceID id) : mEntityID(id) {

		}
		_entityContainer() : mEntityID(0) {}

		bool attachComponent(InstanceID entity_id, const TypeID comp_id) {
			std::pair<_eCSetItor, bool> result = mComponentSet.insert(entity_id);
			//return result.second;
			if(!result.second) return false;
			CTGroupsItor resultT = mTagGroups.find(comp_id);
			if(resultT == mTagGroups.end()) {
				pair<CTGroupsItor, bool> newGroup = mTagGroups.insert(make_pair(comp_id,_componentTagContainer(comp_id)));
				assert(newGroup.second);
				//newGroup.first->add(id);
				newGroup.first->second.add(entity_id);
			} else {
				resultT->second.add(entity_id);
			}
			return true;
		}
		bool detachComponent(const InstanceID entity_id, const TypeID comp_id) {
			_eCSetItor result = mComponentSet.find(entity_id);
			if(result == mComponentSet.end()) return false;
			mComponentSet.erase(result);
			CTGroupsItor resultT = mTagGroups.find(comp_id);
			assert(resultT != mTagGroups.end());
			return resultT->second.remove(entity_id);
		}
		const bool hasComponent(const InstanceID id) const {
			return (mComponentSet.find(id) != mComponentSet.end());
		}
		const bool hasComponentTag(const TypeID tag) const {
			return (mTagGroups.find(tag) != mTagGroups.end());
		}
		const InstanceID getComponent(const TypeID tag) const {
			CTGroupsCItor finder = mTagGroups.find(tag);
			if(finder != mTagGroups.end()) {
				return *finder->second.mComponentSet.begin();
			}
			return 0;
		}
		bool operator==(const _entityContainer& other) {
			return (mEntityID == other.mEntityID);
		}
		bool operator!=(const _entityContainer& other) {
			return (mEntityID != other.mEntityID);
		}
	};
	//struct _entityContainerHasher {
	//	static const btree::b<InstanceID> _ecHasher;
	//	size_t operator()(_entityContainer* ec) {
	//	/*	if(ec) return _ecHasher(ec->mEntityID);
	//		return 0;*/
	//		
	//	}
	//};
	typedef std::unordered_map<InstanceID, _entityContainer> EntityComponentGroups;
	typedef EntityComponentGroups::iterator ECGroupsItor;
	typedef EntityComponentGroups::const_iterator ECGroupsCItor;
	typedef EntityComponentGroups::reverse_iterator ECGroupsRItor;

	struct ComponentEventArgs : public EventArgs {
		InstanceID componentID;
		InstanceID componentParentID;
		TypeID componentType;
		IComponentPtr componentPtr;

		ComponentEventArgs(const InstanceID& cID, const InstanceID& pID, const TypeID& tID, const IComponentPtr& ptr)
			:componentID(cID),
			componentParentID(pID),
			componentType(tID),
			componentPtr(ptr)
		{}
	};
	struct EntityEventArgs: public EventArgs {
		InstanceID entityID;
		EntityPtr entityPtr;

		EntityEventArgs(const InstanceID& eID, const EntityPtr& ptr)
			:entityID(eID),
			entityPtr(ptr)
		{}
	};


	//=======================================================================//
	//ECSManager Class																 //
	//=======================================================================//
	class ECSManager : public enable_shared_from_this<ECSManager> {
		friend class Scene;
		friend class Engine;
	private:
		typedef std::unordered_map<InstanceID, IComponentPtr> ComponentIDMap;
		typedef std::unordered_map<InstanceID, EntityPtr> EntityIDMap;
		typedef std::unordered_multimap<std::string, EntityPtr> EntityTagMap;

		typedef ComponentIDMap::const_iterator CIconst_iterator;
		typedef ComponentIDMap::iterator CIiterator;
		typedef ComponentIDMap::reverse_iterator CIreverse_iterator;

		typedef EntityIDMap::const_iterator EIconst_iterator;
		typedef EntityIDMap::iterator EIiterator;
		typedef EntityIDMap::reverse_iterator EIreverse_iterator;

		typedef EntityTagMap::const_iterator ETconst_iterator;
		typedef EntityTagMap::iterator ETiterator;
		typedef EntityTagMap::reverse_iterator ETreverse_iterator;

		ComponentIDMap mComponentIDMap;
		ComponentTagGroups mComponentTagGroups;

		EntityIDMap mEntityIDMap;
		EntityTagMap mEntityTagMap;
		EntityComponentGroups mEntityComponentGroups;

		SceneID mSceneID;
		//std::recursive_mutex entityAccessMtx;
		//std::recursive_mutex componentAccessMtx;

		//Events
		Event<ComponentEventArgs> ComponentCreateEvent;
		Event<ComponentEventArgs> ComponentDestroyEvent;

		Event<EntityEventArgs> EntityCreateEvent;
		Event<EntityEventArgs> EntityDestroyEvent;
		//Private functionality
		
		void setParentScene(const SceneID& sceneID);
	public:
		ECSManager(const SceneID&parentScene);
		ECSManager();
		~ECSManager();
		SceneID getParentScene();
		/*!
		 * This function will create and attach to the specified entity a component
		 * of the type specified by componentTag. If no component of this type has been
		 * registered, this function will return a nullptr.
		 *\param		componentTag		The type of component that is to be created
		 *\param		parent				A pointer to the Entity that this component
		 *									should be attached to
		 *\param		active				Sets the initial active state for this component
		 *									By default set to true
		 *\return		shared_ptr<Component>			A pointer to a newly created component of the specified type
		 *									or nullptr on failure
		 */
	private:
		IComponentPtr CreateComponentp(const TypeID componentTag, EntityPtrRef parent, bool active = true);
	
		IComponentPtr CreateComponent(const TypeID componentTag, const InstanceID parent, bool active = true);
	public:
		/*!
		 * This function will create and attach to the specified entity a component
		 * of the type specified by componentTag. If no component of this type has been
		 * registered, this function will return a nullptr. 
		 *\param		componentTag		The type of component that is to be created
		 *\param		parent				A pointer to the Entity that this component
		 *									should be attached to
		 *\param		active				Sets the initial active state for this component
		 *									By default set to true
		 *\return		shared_ptr<T>			A pointer to a newly created component of the specified type
		 *									or nullptr on failure
		 */
		template<class T>
		shared_ptr<T> CreateComponentp(EntityPtrRef parent, bool active = true) {
			IComponentPtr nC = CreateComponentp(ctid<T>(), parent, active);
			//nC->setTag(componentTag);
			return dynamic_pointer_cast<T>(nC);			
		}
		template<class T>
		shared_ptr<T> CreateComponent(const InstanceID parent, bool active = true) {
			IComponentPtr nC = CreateComponent(ctid<T>(), parent, active);
			//nC->setTag(componentTag);
			return dynamic_pointer_cast<T>(nC);	
		}
		/*!
		 * This function is in effect a clone-and-paste functions. It accepts a pointer
		 * to a component, which is then cloned (By using the same registration/creation
		 * api that the two other CreateComponent functions do) and attached to the specified
		 * parent Entity. If this function fails to create a component it will return nullptr
		 * and Entity will remain unmodified.
		 *\param		componentPointer	A pointer to the component to clone
		 *\param		parent				Parent entity to attach the component to
		 *\param		active				Sets the initial active state for this component
		 *									By default set to true
		 *
		 *\return		shared_ptr<Component>			A pointer to a newly cloned component or nullptr on failure
		 */
	private:
		IComponentPtr CreateComponent(IComponentPtrRef componentPointer, const InstanceID parent, bool active = true);

		IComponentPtr CreateComponentp(IComponentPtrRef componentPointer, EntityPtrRef parent, bool active = true);
	public:
		/*!
		 * This function is in effect a clone-and-paste functions. It accepts a pointer
		 * to a component, which is then cloned (By using the same registration/creation
		 * api that the two other CreateComponent functions do) and attached to the specified
		 * parent Entity. If this function fails to create a component it will return nullptr
		 * and Entity will remain unmodified.
		 *\param		componentPointer	A pointer to the component to clone
		 *\param		parent				Parent entity to attach the component to
		 *\param		active				Sets the initial active state for this component
		 *									By default set to true
		 *
		 *\return		shared_ptr<T>			A pointer to a newly cloned component or nullptr on failure
		 */
		template<class T>
		shared_ptr<T> CreateComponentp(shared_ptr<T>& componentPointer, EntityPtrRef parent, bool active = true) {
			IComponentPtr nC = CreateComponentp(dynamic_pointer_cast<IComponent>(componentPointer), parent, active); 
			nC->setTag(componentPointer->getCTag());
			return dynamic_pointer_cast<T>(nC);
		}
		template<class T>
		shared_ptr<T> CreateComponent(shared_ptr<T>& componentPointer, const InstanceID parent, bool active = true) {
			IComponentPtr nC = CreateComponent(dynamic_pointer_cast<IComponent>(componentPointer), parent, active); 
			nC->setTag(componentPointer->getCTag());
			return dynamic_pointer_cast<T>(nC);
		}

		//Create/Initialize
		template<class T>
		shared_ptr<T> CreateComponentWithInit(EntityPtrRef parent, 
						std::function<void (IComponentPtrRef)>& initializer, bool active = true) {
			IComponentPtrRef tptr = 
				CreateComponentp(ctid<T>().id, parent, active);
			tptr->setTag(CTID<T>::name);
			if(tptr && initializer) initializer(tptr);
			return dynamic_pointer_cast<T>(tptr);
		}
		template<class T>
		shared_ptr<T> CreateComponentWithInit(shared_ptr<T>& componentPointer, EntityPtrRef parent, 
						std::function<void (IComponentPtrRef)>& initializer, bool active = true) {
			IComponentPtrRef tptr = 
				CreateComponentp(dynamic_pointer_cast<IComponent>(componentPointer), parent, active);
			tptr->setTag(CTID<T>::name);
			if(tptr && initializer) initializer(tptr);
			return dynamic_pointer_cast<T>(tptr);
		}
		/*!
		 * This function is a helpful wrapper around CreateComponent(shared_ptr<Component>, EntityPtr)
		 * It is useful if you need to clone a component but leave it attached to the same parent
		 * This function will simply call GetParent on the specified component and use that as an argument
		 * to CreateComponent. The cloned component will have the same active state as the clonee component
		 * and as such, it's OnActivate will likewise be called appropriately
		 *\param		componentPointer	A pointer to the component to clone
		 *
		 *\return		shared_ptr<Component>			A pointer to a newly cloned component or nullptr on failure
		 */
	private:
		IComponentPtr CloneComponent(IComponentPtrRef componentPointer, const InstanceID entityID = 0);
		
		IComponentPtr CloneComponent(const InstanceID componentID, const InstanceID entityID = 0);
	public:
		/*!
		 * This function is a helpful wrapper around CreateComponent(shared_ptr<Component>, EntityPtr)
		 * It is useful if you need to clone a component but leave it attached to the same parent
		 * This function will simply call GetParent on the specified component and use that as an argument
		 * to CreateComponent. The cloned component will have the same active state as the clonee component
		 * and as such, it's OnActivate will likewise be called appropriately
		 *\param		componentPointer	A pointer to the component to clone
		 *
		 *\return		shared_ptr<T>			A pointer to a newly cloned component or nullptr on failure
		 */
		template<class T>
		shared_ptr<T> CloneComponent(shared_ptr<T>& componentPointer) {
			return dynamic_pointer_cast<T>(CloneComponent(dynamic_pointer_cast<IComponent>(componentPointer)));
		}

		/*!
		 * This function will destroy the component specified by componentPointer by
		 * first removing it from the manager's control, and then detaching it from its parent
		 * entity. Thus a component should be stateful enough that its destructor
		 * can function without knowing it's parent entity
		 *\param		componentPointer	A pointer to the component that is to be destroyed
		 *
		 *\return		bool				A boolean flag that is true on success, false on any failure
		 */
	private:
		bool DestroyComponent(IComponentPtr componentPointer);
		/*!
		 * This function will destroy the first component found with the matching id
		 * by first removing it from the manager's control, and then detaching it from its parent
		 * entity.
		 *\param		componentID			The id of the component to be destroyed
		 *
		 *\return		bool				A boolean flag that is true on success, false on any failure
		 */		
		bool DestroyComponent(const InstanceID componentID);
	public:
		template<class T>
		bool DestroyComponent(shared_ptr<T> componentPtr) {
			static_assert(std::is_base_of<IComponent, T>::value, "Error. Cannot call DestroyComponent on a non-Component!" __FILE__ ":" STR(__LINE__));
			if(!componentPtr) {
				ECS_error("Attempted to destroy a Component via a nullptr");
				return false;
			}
			return DestroyComponent(std::dynamic_pointer_cast<IComponent>(componentPtr));
		}
		/*!
		 * This function will destroy all components with tags matching componentTag
		 * by first removing each from the manager's control, and then detaches each from their
		 * respective parent entities.
		 *\param		componentTag		The component tag to use for lookup
		 *
		 *\return		bool				A boolean flag that is true on success, false on any failure
		 */
		template<typename T>
		bool DestroyAllComponents() {
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call DestroyAllComponents with an invalid type specified");
			return DestroyAllComponents(ctid<T>());
		}
	private:
		bool DestroyAllComponents(const TypeID componentTag);
	public:
		/*!
		 * This function will return a pointer to the first found component with a
		 * matching tag.
		 *\param		componentTag		The component tag to use for lookup
		 *
		 *\return		shared_ptr<Component>			A pointer to the found component, or nullptr on failure
		 */
		template<typename T>
		shared_ptr<T> GetComponent(const InstanceID entityID = 0) {
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call GetComponent on a non-component type" __FILE__ ":" STR(__LINE__));
			return dynamic_pointer_cast<T>(GetComponent(ctid<T>(), entityID));
		}
	private:
		IComponentPtr GetComponent(const TypeID componentTag, const InstanceID entityID = 0);
	public:

		/*template<class T>
		shared_ptr<T> GetComponent(const InstanceID entityID = 0) {
			return GetComponent<T>(ctid<T>(), entityID);
		}*/
			/*!
		 * This function will return a pointer to a component with the matching id
		 *\param		componentID			The id of the component to be found
		 *
		 *\return		shared_ptr<Component>			A pointer to the found component, or nullptr on failure
		 */
	private:
		IComponentPtr GetComponentByID(const InstanceID componentID);
	public:
		/*!
		 * This function will return a pointer to a component with the matching id
		 *\param		componentID			The id of the component to be found
		 *
		 *\return		shared_ptr<Component>			A pointer to the found component, or nullptr on failure
		 */
		template<class T>
		shared_ptr<T> GetComponentByID(const InstanceID componentID) {
			return dynamic_pointer_cast<T>(GetComponent(componentID));
		}
		
		/*!
		 * This function returns true if the manager has a component matching componentPointer
		 * and false otherwise (including any failure)
		 *\param		componentPointer	A pointer to the component that is to be found
		 *
		 *\return		bool				A boolean flag, true if the specified component exists
		 *									in the manager, false otherwise
		 */
	private:
		bool HasComponent(IComponentPtrRef componentPointer);
	public:
		/*!
		 * This function returns true if the manager has a component matching componentPointer
		 * and false otherwise (including any failure)
		 *\param		componentPointer	A pointer to the component that is to be found
		 *
		 *\return		bool				A boolean flag, true if the specified component exists
		 *									in the manager, false otherwise
		 */
		template<class T>
		bool HasComponent(shared_ptr<T> componentPointer) {
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call HasComponent on a non-Component type " __FILE__ ":" STR(__LINE__));
			return HasComponent(dynamic_pointer_cast<IComponent>(componentPointer));
		}
		/*!
		 * This function returns true if the manager has at least one component with a matching
		 * tag, and false otherwise (including any failure)
		 *\param		componentTag		The component tag to use for lookup
		 *
		 *\return		bool				A boolean flag, true if a component with the specified tag
		 *									exists in the manager, false otherwise
		 */
		template<typename T>
		bool HasComponent(const InstanceID entityID = 0) {
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call HasComponent on a non-Component typ " __FILE__ ":" STR(__LINE__));
			return HasComponent(ctid<T>().id, entityID);
		}
	private:
		bool HasComponent(const TypeID componentID, const InstanceID entityID = 0);
	public:
		/*!
		 * This function returns true if the manager has a component with a matching id, and false
		 * otherwise (including any failure)
		 *\param		componentID			The component id to use for lookup
		 *
		 *\return		bool				A boolean flag, true if a component with the specified id
		 *									exists in the manager, false otherwise
		 */
		//bool HasComponentID(const unsigned int componentID, unsigned int entityID = 0);

		//===================================================================//
		//Entity Creation													 //
		//===================================================================//

		/*!
		 * Creates a new blank Entity using the specified tag. The entity is assigned a unique ID
		 * And the specified tag. Fails if provided tag is NULL with no effects. 
		 *\param	entityTag		Tag to use for creating the new Entity
		 *
		 *\return	EntityPtr			Pointer to newly created Entity, or nullptr on failure
		 */
		EntityPtr CreateEntity(const std::string& entityTag);
		/*! Clones the specified entity, assigning it a new unique id
		 * In addition to cloning the Entity, all Components attached the specified entity are also duplicated
		 * This function does not consider the children of the specified entity. Use CloneEntityTree() if you wish to 
		 * duplicate an entity tree
		 *\param	other			Pointer to entity that is to be duplicated
		 *
		 *\return	EntityPtr			Pointer to duplicate entity
		 */
		EntityPtr CloneEntity(EntityPtrRef other);

	private:
		//Recursively calls itself to clone entity tree
		EntityPtr _cloneEntity(EntityPtrRef other);
	public:

		bool AttachEntity(const InstanceID toAttach, const InstanceID parentID);
		/*!
		 * This function will set an Entity's tag to the new value, moving it
		 * in it's internal lists to properly keep track of it
		 *\param	entityPointer	Pointer to entity to re-tag
		 *\param	newTag			New Entity tag to use
		 *
		 *\return	bool			A boolean value, true on succesful re-tagging, false on
		 *							any failure
		 */
		bool SetTag(EntityPtrRef entityPointer, const std::string& newTag);
		/*! This function will attempt to find an entity matching the specified tag
		 * returning the first one found that matches. Since insertion order is not preserved
		 * there is no guarantee that this function will return the first-inserted entity with the specified tag
		 *\param	entityTag		Entity tag to use for finding a matching Entity
		 *
		 *\return	EntityPtr			Pointer to a found entity, or nullptr on failure, or if no entity with the specified tag was found
		 */
		EntityPtr GetEntity(const std::string& entityTag);

		 /*!
		  * This function will attempt to find an entity matching the specified entity id
		  * returning the pointer to it, or nullptr if such an Entity is not found
		  *\param	entityID		Entity id to use for finding a matching Entity
		  *
		  *\return	EntityPtr		Pointer to a found entity, or a nullptr on failure or if no entity with the specified id was found
		  */
		EntityPtr GetEntity(const InstanceID entityID);
		/*! This function will attempt to find all entities matching the specified tag
		 * returning a STL vector containing pointers to each found Entity
		 * Since insertion order is not preserved, there is no guarantee that the returned vector has Entities
		 * in the order they were inserted in.
		 *\param	entityTag		Entity tag to use for finding a matching Entity
		 *
		 *\return	vector<EntityPtr>	Vector containing pointers to matching entities, or an empty vector if none were found, or there was an error
		 */
	private:
		void GetEntities(const std::string& entityTag, vector<EntityPtr>& out);
	public:
		/*! This function will remove the specified Entity from the manager
		 * and will attempt to destroy the Entity objects. In addition, it will
		 * remove all attached Components from the world's ComponentManager and likewise
		 * attempt to destroy them. This function will first detach this entity from it's
		 * parent entity, and then destroy this entity and it's children
		 * recursively.
		 * Because this function will destroy the pointed-at Entity object, any and all pointers
		 * to it will immediately become invalidated
		 *\param	toDestroy		Pointer to entity that is to be removed and destroyed
		 *
		 *\return	bool			Boolean flag that is true on success, and false on any failure. If this function ever return false
		 *							and the passed-in pointer was not NULL, there is no guarantee of the state of the pointed-at Entity.
		 */
		bool DestroyEntity(EntityPtrRef toDestroy);
	private:
		bool _destroyEntity(EntityPtrRef toDestroy);
	public:
		/*! This function will remove and destroy the Entity with the specified tag
		 * In addition to removing and destroying the Entity object, it will also remove and destroy any components
		 * attached to the found Entity. Because this function is a wrapper
		 * around DestroyEntity(EntityPtr) it will similarly destroy the children of this entity.
		 *\param	id				ID of Entity to destroy
		 *
		 *\return	bool			Boolean flag that is true on success, and false on any failure. If this function ever return false
		 *							and you are certain that the specified Entity ID was valid, then there is no guarantee to the state of 
		 *							the Entity with the specified ID
		 */
		bool DestroyEntity(const InstanceID id);

		bool DestroyEntityComponents(const InstanceID entityID);

		size_t GetNumComponents(const InstanceID entityID = 0);

		void dumpEntityToLog(const EntityPtr entity);
		void dumpComponentToLog(const IComponentPtr component);
		
		//Logger Tag:
		static const std::string& getLogTag() {
			static std::string ECSTag = "ECS";
			return ECSTag;
		}
		static ECSManager* GetECS();


		//Update functionality
		void PreUpdate();
		void Update();
		void PostUpdate();

		//Entity Activity:
		void SetEntityActivity(const InstanceID& entityID, const bool activity);
	};



	class Component : public IComponent {
		friend class ECSManager;
	public:
		Component()
			:IComponent()
		{}
		virtual ~Component() {};

	
		template<class T>
		shared_ptr<T> CreateComponent(const InstanceID entityID = 0) {
			assert(ECSManager::GetECS());
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call CreateComponent for a non-Component type " __FILE__ ":" STR(__LINE__));
			if(entityID == 0) {
				return ECSManager::GetECS()->CreateComponent<T>(this->GetParentID());
			} else {
				return ECSManager::GetECS()->CreateComponent<T>(entityID);
			}
		}

		template<class T>
		shared_ptr<T> CloneComponent(IComponentPtrRef componentPointer, bool self = true) {
			assert(ECSManager::GetECS());
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call CloneComponent for a non-Component type " __FILE__ ":" STR(__LINE__));
			if(self) {
				return ECSManager::GetECS()->CloneComponent<T>(componentPointer, this->GetParentID()); 
			} else {
				return ECSManager::GetECS()->CloneComponent<T>(componentPointer, componentPointer->GetParentID());
			}
		}
		
		template<class T>
		shared_ptr<T> AddComponent() {
			assert(ECSManager::GetECS());
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call AddComponent for a non-Component type " __FILE__ ":" STR(__LINE__));
			return ECSManager::GetECS()->CreateComponent<T>(this->GetParentID());
		}

		template<class T>
		shared_ptr<T> AddComponent(IComponentPtrRef componentPointer, bool self = true) {
			assert(ECSManager::GetECS());
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call AddComponent for a non-Component type " __FILE__ ":" STR(__LINE__));
			if(self) {
				return ECSManager::GetECS()->CloneComponent<T>(
					componentPointer, this->GetParentID());
			} else {
				return ECSManager::GetECS(mID.extra)->CloneComponent<T>(
					componentPointer, componentPointer->GetParentID());
			}
		}
		template<class T>
		bool DestroyComponent(IComponentPtrRef componentPtr) {
			assert(ECSManager::GetECS());
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call DestroyComponent for a non-Component type " __FILE__ ":" STR(__LINE__));
			if(*componentPtr == *this) return false;
			return ECSManager::GetECS()->DestroyComponent(componentPtr->getID());
		}
		template<class T>
		bool HasComponent(const InstanceID entityID = 0) {
			assert(ECSManager::GetECS());
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call HasComponent for a non-Component type " __FILE__ ":" STR(__LINE__));
			return HasComponent(ctid<T>().id, entityID);
		}
		template<class T>
		bool HasComponent(const InstanceID componentTag, const InstanceID entityID = 0) {
			assert(ECSManager::GetECS());
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call HasComponent for a non-Component type " __FILE__ ":" STR(__LINE__));
			if(entityID == 0) {
				return ECSManager::GetECS()->HasComponent<T>(componentTag, this->GetParentID());
			} else {
				return ECSManager::GetECS()->HasComponent<T>(componentTag, entityID); 
			}
		}
		template<class T>
		shared_ptr<T> GetComponent(const InstanceID entityID = 0) { 
			assert(ECSManager::GetECS());
			static_assert(std::is_base_of<IComponent, T>::value, "Attempting to call GetComponent for a non-Component type " __FILE__ ":" STR(__LINE__));
			if(entityID == 0) {
				return ECSManager::GetECS()->GetComponent<T>(this->GetParentID());
			} else {
				return ECSManager::GetECS()->GetComponent<T>(entityID);
			}
		}
		inline shared_ptr<Entity> GetEntity(const InstanceID& entityID) {
			assert(ECSManager::GetECS());
			return ECSManager::GetECS()->GetEntity(entityID);
		}
		inline shared_ptr<Entity> GetEntity(const std::string& tag) {
			assert(ECSManager::GetECS());
			return ECSManager::GetECS()->GetEntity(tag);
		}
		inline EntityPtr CloneEntity(EntityPtrRef entity) {
			assert(ECSManager::GetECS());
			return ECSManager::GetECS()->CloneEntity(entity);
		}
		inline EntityPtr CreateEntity(const std::string& tag) {
			assert(ECSManager::GetECS());
			return ECSManager::GetECS()->CreateEntity(tag);
		}
		inline bool DestroyEntity(const InstanceID& entityID) {
			assert(ECSManager::GetECS());
			if(entityID == GetParentID()) return false; //shouldnt be trying to destroy self from within self
			return ECSManager::GetECS()->DestroyEntity(entityID);
		}
		inline bool DestroyEntity(EntityPtr entity) {
			assert(ECSManager::GetECS());
			if(!entity || entity->getID() == GetParentID()) return false;
			return ECSManager::GetECS()->DestroyEntity(entity);
		}
		inline EntityPtr GetParent() {
			assert(ECSManager::GetECS());
			return ECSManager::GetECS()->GetEntity(GetParentID());
		}

		ECSManager* const GetECS() const;
		Camera* const GetCamera() const;
		Scene* const GetScene() const;
	};
	REGISTER_COMPONENT_WFLAGS(Component, "Component", CREG_FLAGS_ENGCOMP);

}
	



#endif