#pragma once
#ifndef __TIMEWHALE_ENTITY_H_
#define __TIMEWHALE_ENTITY_H_

//#include <unordered_map>
#include <vector>
#include <string>
#include <memory>
#include <functional>
#include <mutex>

#include "cppBtree\btree_map.h"
#include "TimewhaleApi.h"
#include "GameObject.h"
#include "InstanceID.h"
#include "StringUtils.h"

namespace Timewhale {
	using namespace std;

	typedef btree::btree_multimap<std::string, weak_ptr<Component>> ComponentMap;
	typedef btree::btree_map<InstanceID, weak_ptr<Entity>> EntityMap;

	typedef ComponentMap::iterator ComponentMapItor;
	typedef ComponentMap::reverse_iterator ComponentMapRItor;

	typedef EntityMap::iterator EntityMapItor;
	typedef EntityMap::reverse_iterator EntityMapRItor;

	template<class T>
	struct ECS_LResult {
		bool hasResult;
		shared_ptr<T>& result;

		ECS_LResult<T>( bool has = false, shared_ptr<T>& res = shared_ptr<T>()) : hasResult(has), result(res) {}; 

		operator bool() const {
			return hasResult;
		}
		operator shared_ptr<T>&() const {
			return result;
		}

	};

	class Entity : public GameObject {
		friend class ECSManager;
	private:
		weak_ptr<Entity> mParent;
		//ECSManager* mECS;

		Entity(const Entity &);

		void setParent(shared_ptr<Entity>& newParent);
	protected:
		EntityMap mChildren;
	public:
		Entity();
		Entity(const std::string& tag);
		Entity(std::string& tag);
		//Entity(const std::string& tag);
		~Entity();

		//ECSManager* GetECS() const;

		///*!
		//	* This function will return if this entity is currently active. It does not consider this entity's children
		//	*\return		bool			A boolean flag representing this entity's activity status
		//	*/
		//const bool isActive() const;
		///*!
		//	* This function will set this entity and all of its children to active recursively
		//	* In addition, it will activate all components attached to this entity and its children
		//	*/
		//void activate();
		///*!
		//	* This function will set this entity and all of its children to inactive recursively
		//	* In addition, it will deactivate all components attached to this entity and its children
		//	*/
		//void deactivate();
		///*!
		//	* This function will set this entity's and all it's childrens active status to the specified value recursively
		//	* In addition, it will set all attached component's status to the provided value
		//	*\param		enableFlag		Boolean flag to set entities active status to
		//	*/
		//void setActive(bool statusFlag);

		//These are undocumented since I'm not certain they will remain 

//		operator unsigned int();
		
		////
		//// Entity tree management
		////

		// Attaching a single entity
		/*!
			* This function attaches the entity specified by entityPointer to this entity as a child
			* Since Entities can only be created and destroyed via the EntityManager, 
			* we can assume that it is present in the manager. Thus, we do not check for that,
			* but simply attach this entity. That also means that the further
			* attachEntityTree function is now un-needed
			*\param		entityPointer			A pointer to the entity that is to be attached
			*
			*\return		bool					A boolean value, true if attachment succeded, false on failure
			*								If false is returned, the state of either entity is not modified
			*/
		bool attachEntity(shared_ptr<Entity>& entityPointer, shared_ptr<Entity>& parent);

		
		//Entity lookup

		/*!
			* This function returns true if the entity specified by entityPointer is an immediate child of this entity
			* This function will perform a search through this entity's internal lists for the result to ensure proper results
			* as an Entity's parent pointer may be modified outside of the managers control (albeit it should not be)
			*\param		entityPointer			A pointer to the entity to check as child
			*
			*\return		ECS_LResult<Entity>		A structure containing lookup results
			*/
		ECS_LResult<Entity> hasChild(shared_ptr<Entity>& entityPointer);

		/*!
			* This function returns true if this entity has at least one immediate child entity with the specified tag. There is an
			* optional parameter to store the pointer to the matching entity which is set to nullptr by default. 
			*\param		entityTag				The entity tag to check as child
			*
			*\return		ECS_LResult<Entity>		A structure containing lookup results
			*/
		ECS_LResult<Entity> hasChild(const std::string& entityTag);

		/*!
			* This function returns true if this entity has an immediate child entity with a matching ID. There is an 
			* optional parameter to store the pointer to the matching entity which is set to nullptr by default
			*\param		entityID				The entity id to check as child
			*
			*\return		ECS_LResult<Entity>		A structure containing lookup results
			*/
		ECS_LResult<Entity> hasChild(const InstanceID entityID);

		ECS_LResult<Entity> hasChild(function<bool (shared_ptr<Entity>)>& testFunc);

		/*!
			* This function will return a pointer to an immediate child entity that matches the one provided by entityPointer
			* Although this function is nominally a pass-through, if the entity specified by entityPointer is not an immediate
			* child of this entity, this function will return nullptr. 
			*\param		entityPointer			A pointer to the entity to get
			*
			*\return		Entity*				A pointer to the found child entity, or nullptr if one could not be found
			*/
		shared_ptr<Entity> getChild(shared_ptr<Entity>& entityPointer);

		/*!
			* This function will return a pointer to the first immediate child entity of this entity that matches the specified tag
			*\param		entityTag				The entity tag to use for finding a child entity
			*
			*\return		Entity*				A pointer to the found child entity, or nullptr if one could not be found
			*/
		shared_ptr<Entity> getChild(const std::string& entityTag);

		/*!
			* This function will return a pointer to the immediate child entity of this entity that matches the specified id
			*\param		entityID				The entity id to use for finding a child entity
			*
			*\return		Entity*				A pointer to the found child entity, or nullptr if one could not be found
			*/
		shared_ptr<Entity> getChild(const InstanceID entityID);

		shared_ptr<Entity> getChild(function<bool (shared_ptr<Entity>)>& testFunc);

		/*!
			* This function returns a vector of all entities that are immediate children of this entity and match the specified tag
			* This function will always return a vector, even if it is empty
			*\param		entityTag				The entity tag to use for finding child entities
			*
			*\return		vecotr<Entity*>			A vector of entity pointers containing all entities that match the specified tag
			*								and are immediate children of this entity
			*/
		void getAllChildren(const std::string& entityTag, vector<shared_ptr<Entity>>& out);

		void getAllChildrenTest(function<bool (shared_ptr<Entity>)>& testFunc, vector<shared_ptr<Entity>>& out);

		/*!
			* This function returns a vector of entity pointers containing pointers to all immediate children of this entity
			*\return		vecotr<Entity*>			A vector of pointers to all immediate child entities of this entity
			*/
		void getAllChildren(vector<shared_ptr<Entity>>& out);

		/*!
			* This function returns a vector of all immediate child entity tags. The vector only contains unique tags (that is, no duplicates)
			*\return		vector<wchar_t*>		A vector of all unique entity tags for all immediate child entities of this entity
			*/
		void getAllChildTags(vector<std::string>& out);

		/*!
			* This function returns a vector of all immediate child entity ids.
			*\return		vector<unsigned int>	A vector of entity id's for all immediate child entities of this one
			*/
		void getAllChildIds(vector<InstanceID>& out);

		/*!
			* This function will detach the specified entity from this entity, and return a pointer to it, or nullptr
			* if the specified entity is not an immediate child of this one.. The detached entity will not have its state
			* altered beyond changing it's parent pointer.
			*\param		entityPointer			A pointer to the entity to detach
			*
			*\return		Entity*				A pointer to the entity detached from this entity, or nullptr if
			*								the specified entity is not an immediate child of this entity
			*/
		shared_ptr<Entity> detachChildEntity(shared_ptr<Entity>& entityPointer);

		/*!
			* This function will detach the entity specified by entityID, and return a pointer to it, or nullptr
			* if an entity with the specified id could not be found as an immediate child of this one. The detached
			* entity will not have its state altered beyond changing it's parent pointer
			*\param		entityID				The entity id to use to find a child entity to detach
			*
			*\return		Entity*				A pointer to the entity detached from this entity with a matching id
			*								or nullptr if an entity with the specified id could not be found as
			*								an immediate child of this one
			*/
		shared_ptr<Entity> detachChildEntity(const InstanceID entityID);

		shared_ptr<Entity> detachChildEntity(function<bool (shared_ptr<Entity>)>& testFunc);

		/*!
			* This function will detach all immediate child entities of this one and return a vector of pointers to
			* the detached entities, or an empty vector if this entity had no children
			*\return		vector<Entity*>			A vector of entity pointers that were detached from this entity
		*/
		void detachAllChildEntities();

		//counter getters
		const size_t getNumChildren() const;

		shared_ptr<Entity> getParent();
		const shared_ptr<Entity> getParent() const;

		void Activate();
		void Deactivate();
		void SetActive(const bool active);

	};
}

#endif