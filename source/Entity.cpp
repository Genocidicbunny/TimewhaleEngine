#include "Entity.h"
#include "cppBtree\btree_set.h"

namespace Timewhale {

	Entity::Entity() 
		: GameObject(), 
		mParent()
	{
		//We should log that an Entity with MissingTag is being created

	}
	Entity::Entity(const std::string& tag)
		:GameObject(tag),
		mParent()
	{}

	Entity::Entity(std::string& tag) 
		:GameObject(tag),
		mParent()
	{}
		
	//Copies position, but does not set the parent, nor adds any children or components
	//The EntityManager handles that
	Entity::Entity(const Entity &that) 
		: GameObject(that),
		mParent()
	{
	}

	Entity::~Entity() {
		this->detachAllChildEntities();
		mChildren.clear();
	}

	/*const bool Entity::operator==(const unsigned int other) const{
		return mID == other;
	}
	const bool Entity::operator!=(const unsigned int other) const{
		return mID != other;
	}*/

	////
	//// Entity tree management
	////

	bool Entity::attachEntity(shared_ptr<Entity>& entityPointer, shared_ptr<Entity>& parent) {
		if(!entityPointer) return false;
		if(!parent || !(parent->getID() == this->getID())) return false;
		//set the entity's parent to this entity
		entityPointer->setParent(parent);
		//insert entity into child list
		mChildren.insert(make_pair(entityPointer->getID(), weak_ptr<Entity>(entityPointer)));
		//finally, set the entity's active status to this entity's active status
		//the entity should now be active, if it's not something went wrong
		return true;
	}

	//Entity lookup

	ECS_LResult<Entity> Entity::hasChild(shared_ptr<Entity>& entityPointer) {
		if(!entityPointer) return ECS_LResult<Entity>();
		EntityMapItor foundEntity = mChildren.find(entityPointer->getID());
		if(foundEntity != mChildren.end()) return ECS_LResult<Entity>(true, entityPointer);
		return ECS_LResult<Entity>();
	}

	ECS_LResult<Entity> Entity::hasChild(const std::string& entityTag){
		if(entityTag.length() == 0) {
			return ECS_LResult<Entity>();
		}
		for(auto pE : mChildren) {
			shared_ptr<Entity> pEs(pE.second);
			if(pEs && pEs->compareTag(entityTag)) {
				return ECS_LResult<Entity>(true, pEs);
			}
		}
		return ECS_LResult<Entity>();
	}

	ECS_LResult<Entity> Entity::hasChild(const InstanceID entityID) {
		EntityMapItor foundEntity = mChildren.find(entityID);
		if(foundEntity == mChildren.end()) {
			return ECS_LResult<Entity>();
		}
		return ECS_LResult<Entity>(true, shared_ptr<Entity>(foundEntity->second));
	}

	ECS_LResult<Entity> Entity::hasChild(function<bool (shared_ptr<Entity>)>& testFunc) {
		if(!testFunc) return ECS_LResult<Entity>();
		EntityMapItor walker = mChildren.begin();
		while(walker != mChildren.end()) {
			shared_ptr<Entity> walkerS(walker->second);
			if(walkerS && testFunc(walkerS)) {
				return ECS_LResult<Entity>(true, walkerS);
			}
		}
		return ECS_LResult<Entity>();
	}

	shared_ptr<Entity> Entity::getChild(shared_ptr<Entity>& entityPointer) {
		if(!entityPointer) return shared_ptr<Entity>();
		bool foundRes = this->hasChild(entityPointer);
		if(foundRes) return entityPointer;
		return shared_ptr<Entity>();
	}

	shared_ptr<Entity> Entity::getChild(const std::string& entityTag) {
		if(entityTag.length() == 0 ) return shared_ptr<Entity>();
		//ECS_LResult<Entity> foundRes = this->hasChild(entityTag);
		//return foundRes;
		return this->hasChild(entityTag);
	}

	shared_ptr<Entity> Entity::getChild(const InstanceID entityID) {
		ECS_LResult<Entity> foundRes = this->hasChild(entityID);
		return foundRes;
	}

	shared_ptr<Entity> Entity::getChild(function<bool (shared_ptr<Entity>)>& testFunc) { 
		return hasChild(testFunc);
	}

	void Entity::getAllChildren(const std::string& entityTag, vector<shared_ptr<Entity>>& out) {
		if(entityTag.length() == 0) return; //make_shared<vector<shared_ptr<Entity>>>();
		//shared_ptr<vector<shared_ptr<Entity>>> resultVec = make_shared<vector<shared_ptr<Entity>>>();
		for(auto pE : mChildren) {
			shared_ptr<Entity> pEs(pE.second);
			if(pEs && pEs->compareTag(entityTag)) {
				out.push_back(pEs);
			}
		}
	}

	void Entity::getAllChildren(vector<shared_ptr<Entity>>& out) {
		for(auto pE : mChildren) {
			shared_ptr<Entity> pEs(pE.second);
			if(pEs) {
				out.push_back(pEs);
			}
		}
	}

	void Entity::getAllChildrenTest(function<bool (shared_ptr<Entity>)>& testFunc, vector<shared_ptr<Entity>>& out) {
		if(!testFunc) return;// make_shared<vector<shared_ptr<Entity>>>();
		//shared_ptr<vector<shared_ptr<Entity>>> resultVec = make_shared<vector<shared_ptr<Entity>>>();
		EntityMapItor walker = mChildren.begin();
		while(walker != mChildren.end()) {
			shared_ptr<Entity> walkerS(walker->second);
			if(walkerS && testFunc(walkerS)) {
				out.push_back(walkerS);
			}
		}
	}

	void Entity::getAllChildTags(vector<std::string>& out) {
		btree::btree_set<std::string> resultSet;
		for(auto pE : mChildren) {
			shared_ptr<Entity> pEs(pE.second);
			if(pEs) {
				auto found =
					resultSet.insert(pEs->getCTag());
			}
		}
		for(const std::string& tag : resultSet) {
			out.push_back(tag);
		}
		//return rsultVec;

	}

	void Entity::getAllChildIds(vector<InstanceID>& out) {
		for(auto pE : mChildren) {
			out.push_back(pE.first);
		}
	}

	shared_ptr<Entity> Entity::detachChildEntity(shared_ptr<Entity>& entityPointer) {
		if(!entityPointer) return shared_ptr<Entity>();
		return this->detachChildEntity(entityPointer->getID());
	}

	shared_ptr<Entity> Entity::detachChildEntity(const InstanceID entityID) {
		EntityMapItor foundEntity = mChildren.find(entityID);
		if(foundEntity != mChildren.end()) {
			shared_ptr<Entity> detached(foundEntity->second);
			if(detached) {
				detached->setParent(shared_ptr<Entity>());
			}
			mChildren.erase(foundEntity);
			return detached;
		}
		return shared_ptr<Entity>();
	}

	shared_ptr<Entity> Entity::detachChildEntity(function<bool (shared_ptr<Entity>)>& testFunc) {
		if(!testFunc) return shared_ptr<Entity>();
		EntityMapItor walker = mChildren.begin();
		while(walker != mChildren.end()) {
			shared_ptr<Entity> walkerS(walker->second);
			if(walkerS && testFunc(walkerS)) {
				return detachChildEntity(walkerS);
			}
		}
		return shared_ptr<Entity>();
	}

	void Entity::detachAllChildEntities() {
		//shared_ptr<Entity>  detached;
		for(auto pE : mChildren) {
			shared_ptr<Entity> pEs(pE.second);
			if(pEs) {
				pEs->setParent(shared_ptr<Entity>());
			}
		}
		mChildren.clear();
	}

	//counter getters
	const size_t Entity::getNumChildren() const {
		return unsigned int(mChildren.size());
	}


	shared_ptr<Entity> Entity::getParent() {
		return shared_ptr<Entity>(mParent);
	}
	//shared_ptr<Entity> Entity::getParent() {
	//	return shared_ptr<Entity>(mParent);
	//}

	void Entity::setParent(shared_ptr<Entity>& newParent) {
		mParent = newParent;
	}

	//ECSManager* Entity::GetECS() const{
	//	//std::lock_guard<const std::recursive_mutex> lock(entityMtx);
	//	return mECS;
	//}
}
