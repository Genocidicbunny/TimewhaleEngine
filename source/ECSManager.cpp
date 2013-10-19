#include <cassert>
#include "ECS.h"
//#include "TransformComponent.h"
#include "TWLogger.h"
#include "StringUtils.h"
#include "InstanceID.h"
#include "SceneManager.h"
#include "Camera.h"

namespace Timewhale {
		
		ECSManager::ECSManager(const SceneID& parentScene) : mSceneID(parentScene) 
		{
			ECS_info("ECSManager Created");
			if(!parentScene) ECS_error("ECSManager created with a null parent Scene. Further behaviour is undefined!");
		}
		ECSManager::ECSManager()
			:mSceneID(0)
		{
			ECS_info("ECSManager created. No Scene bound!");
			ECS_warn("ECSManager created with a nullptr to a Scene.");
		}

		ECSManager::~ECSManager() {
			ECS_info("Destroying ECSManager");
			ECS_debug("Components Left %d", mComponentIDMap.size());
			
			ECS_debug("Entities Left %d", mEntityIDMap.size());
			std::vector<IComponentPtr> scratchVec;
			for(auto p : mEntityIDMap) {
				dumpEntityToLog(p.second);
				DestroyEntityComponents(p.second->getID());
				EntityDestroyEvent.Call(
					EntityEventArgs(p.second->getID(), p.second));
			}
			for(auto p : mComponentIDMap) {
				dumpComponentToLog(p.second);
				p.second->OnDestroy();
				ComponentDestroyEvent.Call(
					ComponentEventArgs(	p.second->getID(),
										p.second->GetParentID(),
										p.second->mTypeID,
										p.second));
			}
			mComponentIDMap.clear();
			mEntityIDMap.clear();
			mEntityTagMap.clear();
			mComponentTagGroups.clear();
			mEntityComponentGroups.clear();
			ECS_info("ECSManager Destroyed");
		}

		void ECSManager::setParentScene(const SceneID& sceneID) {
			ECS_debug("Parent Scene From: %10d To: %10d", mSceneID.id, sceneID.id);
			mSceneID = sceneID;
		}

		SceneID ECSManager::getParentScene() {
			return mSceneID;
		}
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
		IComponentPtr ECSManager::CreateComponent(const TypeID componentTag, const InstanceID parent, bool active) {
			ECS_info("Creating Component. TypeID: %10d, Parent.ID: %10d, Active: %s", componentTag.typeID, parent.id, BoolToStr(active));
			if(!parent) return IComponentPtr();
			CRegMap& reg = GetComponentRegMap();
			CRegMap::iterator it = reg.find(componentTag);
			
			if(it != reg.end()) {
				ECS_debug("Found constructor map for TypeID %10d", componentTag.typeID);
				ECS_debug("Creating dependencies for TypeID %10d", componentTag.typeID);
				//Check that we have all of the components necessary for this one
				ComponentDepsMap& cdMap = GetDepsMap();
				ComponentDepsMap::iterator cdIT = cdMap.find(componentTag);
				if(cdIT != cdMap.end()) {
					ECS_debug("Got Component dependency map for TypeID %10d", componentTag.typeID);
					ECGroupsItor depsE = mEntityComponentGroups.find(parent);
					assert(depsE != mEntityComponentGroups.end());
					_entityContainer& ecg = depsE->second;
					CDepList& cdlist = cdIT->second;
					for(auto dep : cdlist.deps) {
						if(!ecg.hasComponentTag(dep)) {
							ECS_debug("Creating Dependency with TypeID %10d for TypeID %10d", dep, componentTag.typeID); 
							CreateComponent(dep, parent, active);
						}
					}
				}
				//call Component Creation function
				CreateComponentFunc func = it->second;
				ECS_debug("Found component constructor in map at address %p", func);
				assert(func);
				IComponentPtr newComponent = func();
				
				if(newComponent) {
					ECS_info("Component Created. TypeID: %10d ID: %10d Ptr: %p", componentTag.typeID, newComponent->getID(), newComponent.get());
					InstanceID cID = newComponent->getID();
					//unsigned int pID = parent->getID();
					//Set flags from mapping
					ECS_debug("Getting Component flags for TypeID %10d", componentTag.typeID);
					CRegFlagMap& fmap = GetComponentFlagMap();
					CRegFlagMap::iterator walker = fmap.find(componentTag);
					if(walker != fmap.end()) {
						ECS_debug("Flags found for TypeID %10d", componentTag.typeID);
						newComponent->SetFlags(walker->second);
					}
					//Set component Tag
					newComponent->setTag("IComponent");
					newComponent->mTypeID = componentTag;

					//attach component to parent
					ECGroupsItor parenter = mEntityComponentGroups.find(parent);
					assert(parenter != mEntityComponentGroups.end());
					bool res = parenter->second.attachComponent(cID, componentTag);
					if(res) {
						newComponent->SetParent(parent);
						ECS_debug("Component parent set to ID: %10d", parent.id);
					}

					//This should now be done automatically when you add to the entity
					////Add to Entity's tag groups
					//ComponentTagGroups& parentTags = parenter->second.mTagGroups;
					//CTGroupsItor entityTagger = parentTags.find(newComponent->getCTag());
					//if(entityTagger == parentTags.end()) {
					//	pair<CTGroupsItor, bool> newEntityTagger = parentTags.emplace(make_pair(newComponent->getCTag(), _componentTagContainer(newComponent->getCTag())));
					//	assert(newEntityTagger.second);
					//	newEntityTagger.first->second.add(cID);
					//}

					//Add component to tag group
					CTGroupsItor tagger = mComponentTagGroups.find(/*newComponent->getCTag()*/ newComponent->mTypeID);
					if(tagger == mComponentTagGroups.end()) {
						ECS_debug("No existing component tag group found for TypeID %10d", componentTag.typeID);
						auto newTagger = mComponentTagGroups.insert(make_pair(newComponent->mTypeID, _componentTagContainer(newComponent->mTypeID)));
						assert(newTagger.second);
						newTagger.first->second.add(cID);
					} else {
						ECS_debug("Found existing component tag group, inserting into it");
						tagger->second.add(cID);
					}

					//Add component to ID map
					auto idPlacer = mComponentIDMap.insert(make_pair(cID, newComponent));
					assert(idPlacer.second);
					shared_ptr<Component> newComp = dynamic_pointer_cast<Component>(newComponent);

					//newComp->mECS = this; 
					//componentAccessMtx.unlock();
					//neComponent->SetParent(parent);
					ECS_debug("Initializing Component. ID: %10d", newComponent->getID().id);

					newComponent->mID.extra = mSceneID.id;
					//Component Creation Event
					ComponentCreateEvent.Call(
						ComponentEventArgs(	newComponent->getID(), 
											newComponent->GetParentID(), 
											newComponent->mTypeID, 
											newComponent));

					newComponent->Init();
					newComponent->OnActivate();
					return newComponent;
				}
			} else {
				ECS_error("Constructor for Component with TypeID %10d not found! Check that the component is registered", componentTag.typeID);
			}
			return IComponentPtr();
		}

		IComponentPtr ECSManager::CreateComponentp(const TypeID componentTag, EntityPtrRef parent, bool active) {
			if(!parent) {
				ECS_error("Attempted to create unbound Component with TypeID %10d. Component will not be created.", componentTag.typeID);
				return IComponentPtr();
			}
			return CreateComponent(componentTag, parent->getID());
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
		IComponentPtr ECSManager::CreateComponentp(IComponentPtrRef componentPointer, EntityPtrRef parent, bool active) {
			if(!componentPointer) { 
				ECS_error("Attempted to create a component from a Null component pointer. Component will not be created!");
				return IComponentPtr();
			}
			return CreateComponentp(componentPointer->mTypeID, parent, active);
		}
		IComponentPtr ECSManager::CreateComponent(IComponentPtrRef componentPointer, const InstanceID parent, bool active) {
			if(!componentPointer) { 
				ECS_error("Attempted to create a component from a Null component pointer. Component will not be created!");
				return IComponentPtr();
			}
			return CreateComponent(componentPointer->mTypeID, parent, active);

		}
		
		IComponentPtr ECSManager::CloneComponent(const InstanceID componentID, const InstanceID entityID) {
			//componentAccessMtx.lock();
			CIiterator finder = mComponentIDMap.find(componentID);
			
			if(finder == mComponentIDMap.end()) {
			//	componentAccessMtx.unlock();
				ECS_error("Could not find a Component with ID %10d to Clone!", componentID.id);
				return IComponentPtr();
			}
			//componentAccessMtx.unlock();
			return CloneComponent(finder->second, entityID);
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
		IComponentPtr ECSManager::CloneComponent(IComponentPtrRef componentPointer, const InstanceID entityID) {
			if(!componentPointer) { 
				ECS_error("Attempted to create a component from a Null component pointer. Component will not be created!");
				return IComponentPtr();
			}
			ECS_info("Cloning component with TypeID %10d, Parent.ID: %10d", componentPointer->mTypeID.typeID, entityID.id);
			if(entityID == error_iid) {
				return CreateComponent(componentPointer->mTypeID, componentPointer->GetParentID());
			} else {
				return CreateComponent(componentPointer->mTypeID, entityID);
			}
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
		bool ECSManager::DestroyComponent(IComponentPtr componentPointer) {
			if(!componentPointer) {
				ECS_error("Component destruction failed. The Component handle was null");
				return false;
			}
			InstanceID pID = componentPointer->GetParentID();

			ECS_info("Destroying component with TypeID %10d. ID: %10d, Parent.ID: %10d", componentPointer->mTypeID, componentPointer->getID().id, pID.id);

			ECGroupsItor parenter = mEntityComponentGroups.find(pID);
			assert(parenter != mEntityComponentGroups.end());
			parenter->second.detachComponent(componentPointer->getID(), componentPointer->mTypeID);

			CTGroupsItor tagger = mComponentTagGroups.find(componentPointer->mTypeID);
			assert(tagger != mComponentTagGroups.end());
			tagger->second.remove(componentPointer->getID());

			CIiterator idFinder = mComponentIDMap.find(componentPointer->getID());
			assert(idFinder != mComponentIDMap.end());

			ComponentDestroyEvent.Call(
				ComponentEventArgs(	componentPointer->getID(), 
									componentPointer->GetParentID(), 
									componentPointer->mTypeID, 
									componentPointer));
			componentPointer->OnDestroy();
			mComponentIDMap.erase(idFinder);
			return true;
		}
		/*!
		 * This function will destroy the first component found with the matching id
		 * by first removing it from the manager's control, and then detaching it from its parent
		 * entity.
		 *\param		componentID			The id of the component to be destroyed
		 *
		 *\return		bool				A boolean flag that is true on success, false on any failure
		 */		
		bool ECSManager::DestroyComponent(const InstanceID componentID) {
			CIiterator idFinder = mComponentIDMap.find(componentID);
			if(idFinder == mComponentIDMap.end()) {
				ECS_error("Could not find a Component with ID %10d.", componentID.id);
				return false;
			}
			auto componentPointer = idFinder->second;

			InstanceID pID = componentPointer->GetParentID();
			ECS_info("Destroying component with TypeID %10d. ID: %10d, Parent.ID: %10d", 
				componentPointer->mTypeID.typeID, componentPointer->getID().id, pID.id);

			ECGroupsItor parenter = mEntityComponentGroups.find(pID);
			assert(parenter != mEntityComponentGroups.end());
			parenter->second.detachComponent(componentID, componentPointer->mTypeID);

			CTGroupsItor tagger = mComponentTagGroups.find(componentPointer->mTypeID);
			assert(tagger != mComponentTagGroups.end());
			tagger->second.remove(componentID);
			componentPointer->OnDestroy();
			ComponentDestroyEvent.Call(
				ComponentEventArgs(	componentPointer->getID(), 
									componentPointer->GetParentID(), 
									componentPointer->mTypeID, 
									componentPointer));
			mComponentIDMap.erase(idFinder);
			return true;
		}

		/*!
		 * This function will destroy all components with tags matching componentTag
		 * by first removing each from the manager's control, and then detaches each from their
		 * respective parent entities.
		 *\param		componentTag		The component tag to use for lookup
		 *
		 *\return		bool				A boolean flag that is true on success, false on any failure
		 */
		bool ECSManager::DestroyAllComponents(const TypeID typeID) {
			CTGroupsItor tagFinder = mComponentTagGroups.find(typeID);
			CIiterator idFinder;
			ECGroupsItor parenter;
			if(tagFinder == mComponentTagGroups.end()) { 
				ECS_warn("Could not find any components with TypeID: %10d", typeID.typeID);	
				return false;
			}
			for( auto componentID : tagFinder->second.mComponentSet) {
				idFinder = mComponentIDMap.find(componentID);
				assert(idFinder != mComponentIDMap.end());
				InstanceID pID = idFinder->second->GetParentID();

				ECS_info("Found Component with ID %10d to destroy. Parent.ID: %10d", componentID.id, pID.id);

				parenter = mEntityComponentGroups.find(pID);
				assert(parenter != mEntityComponentGroups.end());
				parenter->second.detachComponent(componentID, idFinder->second->mTypeID);				
				auto componentPointer = idFinder->second;
				ComponentDestroyEvent.Call(
					ComponentEventArgs(	componentPointer->getID(), 
										componentPointer->GetParentID(), 
										componentPointer->mTypeID, 
										componentPointer));
				idFinder->second->OnDestroy();
				mComponentIDMap.erase(idFinder);
			}
			tagFinder->second.make_empty();
			ECS_info("All components with TypeID %10d destroyed", typeID.typeID);
			return true;
		}
		/*!
		 * This function will return a pointer to the first found component with a
		 * matching tag.
		 *\param		componentTag		The component tag to use for lookup
		 *
		 *\return		shared_ptr<Component>			A pointer to the found component, or nullptr on failure
		 */
		IComponentPtr ECSManager::GetComponent(const TypeID componentTag, const InstanceID entityID) {
			ECS_info("Getting Component with TypeID %10d from Parent %10d", componentTag.typeID, entityID.id);
			if(entityID == error_iid) {
				CTGroupsItor tagFinder = mComponentTagGroups.find(componentTag);
				if(tagFinder == mComponentTagGroups.end()) { 
					ECS_error("Could not find a Component with TypeID %10d attached to Parent %10d", componentTag.typeID, entityID.id);	
					return IComponentPtr();
				}
				_componentTagContainer& tr = tagFinder->second;
				_cTSetItor begin = tr.mComponentSet.begin();
				if(begin == tr.mComponentSet.end()) return IComponentPtr();
				return mComponentIDMap.find(*begin)->second;
			} else {
				ECGroupsItor entityFinder = mEntityComponentGroups.find(entityID);
				if(entityFinder == mEntityComponentGroups.end()){
					ECS_error("Could not find Entity with ID %10d. Cannot GetComponent", entityID.id);
					return IComponentPtr();
				}

				ComponentTagGroups& cg = entityFinder->second.mTagGroups;
				CTGroupsItor tagFinder = cg.find(componentTag);
				if(tagFinder == cg.end()) { 
					ECS_error("Could not find a Component with TypeID %10d attached to parent %10d", componentTag.typeID, entityID.id);
					return IComponentPtr();
				}
				_componentTagContainer& tr = tagFinder->second;
				_cTSetItor begin = tr.mComponentSet.begin();
				if(begin == tr.mComponentSet.end()) { 
					ECS_error("Could not find a Component with TypeID %10d in Parent %10d tag container", componentTag.typeID, entityID.id);	
					return IComponentPtr();
				}
				return mComponentIDMap.find(*begin)->second;
			}
		}
		/*!
		 * This function will return a pointer to a component with the matching id
		 *\param		componentID			The id of the component to be found
		 *
		 *\return		shared_ptr<Component>			A pointer to the found component, or nullptr on failure
		 */
		IComponentPtr ECSManager::GetComponentByID(const InstanceID componentID) {
			//std::lock_guard<std::recursive_mutex> lock(componentAccessMtx);
			CIiterator finder = mComponentIDMap.find(componentID);
			if(finder == mComponentIDMap.end()) {
				ECS_error("Could not find a Component with ID %10d", componentID.id);
				return IComponentPtr();
			}
			return finder->second;
		}
		/*!
		 *	This function returns a vector of pointers to components that match the specified tag
		 *\param		componentTag		The component tag to use for lookup
		 *
		 *\return		vector<shared_ptr<Component>>	A vector of component pointers, or an empty vector on failure
		 */
		//shared_ptr<vector<IComponentPtr>> ECSManager::GetAllComponents(const std::type_index& componentTag) {
		//	//if(componentTag.length() == 0) return make_shared<vector<IComponentPtr>>();
		//	//componentAccessMtx.lock();
		//	shared_ptr<vector<IComponentPtr>> resultVec = make_shared<vector<IComponentPtr>>();
		//	CTGroupsItor finder = mComponentTagGroups.find(componentTag);
		//	if(finder == mComponentTagGroups.end()) return resultVec;
		//	for(const unsigned int componentID : finder->second.mComponentSet) {
		//		resultVec->push_back(mComponentIDMap.find(componentID)->second);
		//	}
		//	//componentAccessMtx.unlock();
		//	return resultVec;
		//}
		/*!
		 * This function returns true if the manager has a component matching componentPointer
		 * and false otherwise (including any failure)
		 *\param		componentPointer	A pointer to the component that is to be found
		 *
		 *\return		bool				A boolean flag, true if the specified component exists
		 *									in the manager, false otherwise
		 */
		bool ECSManager::HasComponent(IComponentPtrRef componentPointer) {
			if(!componentPointer) { 
				ECS_error("Attempting to call HasComponent on a nullptr");
				return false;
			}
			//std::lock_guard<std::recursive_mutex> lock(componentAccessMtx);
			CIiterator finder = mComponentIDMap.find(componentPointer->getID());
			return finder == mComponentIDMap.end();
		}
		/*!
		 * This function returns true if the manager has at least one component with a matching
		 * tag, and false otherwise (including any failure)
		 *\param		componentTag		The component tag to use for lookup
		 *
		 *\return		bool				A boolean flag, true if a component with the specified tag
		 *									exists in the manager, false otherwise
		 */
		bool ECSManager::HasComponent(const TypeID componentTag, const InstanceID entityID ) {
			if(entityID == error_iid) {
				CTGroupsItor finder = mComponentTagGroups.find(componentTag);
				if(finder == mComponentTagGroups.end()) return false;
				return finder->second.mComponentSet.size() > 0;
			} else {
				ECGroupsItor eFinder = mEntityComponentGroups.find(entityID);
				if(eFinder == mEntityComponentGroups.end()) {
					ECS_error("Could not find Entity with ID %10d", entityID.id);
					return false;
				}

				CTGroupsItor finder = eFinder->second.mTagGroups.find(componentTag);
				if(finder == eFinder->second.mTagGroups.end()) { 
					ECS_error("Could not find Component with TypeID %10d attached to Parent %10d", componentTag.typeID, entityID.id);
					return false;
				}
				return finder->second.mComponentSet.size() > 0;
			}
		}
		/*!
		 * This function returns true if the manager has a component with a matching id, and false
		 * otherwise (including any failure)
		 *\param		componentID			The component id to use for lookup
		 *
		 *\return		bool				A boolean flag, true if a component with the specified id
		 *									exists in the manager, false otherwise
		 */
		//bool ECSManager::HasComponentID(const unsigned int componentID, unsigned int entityID) {
		//	if(entityID == 0) {
		//	//	std::lock_guard<std::recursive_mutex> lock(componentAccessMtx);
		//		return mComponentIDMap.find(componentID) != mComponentIDMap.end();
		//	} else {
		//	//	entityAccessMtx.lock();
		//		ECGroupsItor eFinder = mEntityComponentGroups.find(entityID);
		//		if(eFinder == mEntityComponentGroups.end()) {
		//			//entityAccessMtx.unlock();
		//			return false;
		//		}
		//		//entityAccessMtx.unlock();
		//		//std::lock_guard<std::recursive_mutex> lock(componentAccessMtx);
		//		return (eFinder->second.mComponentSet.find(componentID) != 
		//				eFinder->second.mComponentSet.end());
		//	}
		//}

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
		EntityPtr ECSManager::CreateEntity(const std::string& entityTag) {
			if(entityTag.length() == 0) { 
				ECS_error("Attempting to create an Entity with an empty Tag!");
				return EntityPtr();
			}
			EntityPtr newEntity = make_shared<Entity>(entityTag);
			if(newEntity) {
				auto groupPlacer = 
					mEntityComponentGroups.insert(make_pair(newEntity->getID(), _entityContainer(newEntity->getID())));
				ECS_info("Generated a new Component Group for Entity with ID: %10d", newEntity->getID().id);
				assert(groupPlacer.second);
				auto idPlacer =
					mEntityIDMap.insert( make_pair(newEntity->getID(), newEntity));
				ECS_info("Generated a new Entity entry in the Entity ID map");
				assert(idPlacer.second);
				ETiterator tagPlacer = mEntityTagMap.insert(make_pair(entityTag, newEntity));
				assert(tagPlacer != mEntityTagMap.end());
				ECS_info("Generated new Entity entry in the Entity Tag map");
				//newEntity->mECS = this;
				//Set the scene ID:
				newEntity->mID.extra = mSceneID.id;

				EntityCreateEvent(
					EntityEventArgs(newEntity->getID(), newEntity));
				return newEntity;
			} else {
				ECS_error("Critical! Entity Creation failed! This likely means we're out of memory or IDs!");
				return EntityPtr();
			}
		}
		/*! Clones the specified entity, assigning it a new unique id
		 * In addition to cloning the Entity, all Components attached the specified entity are also duplicated
		 * This function does not consider the children of the specified entity. Use CloneEntityTree() if you wish to 
		 * duplicate an entity tree
		 *\param	other			Pointer to entity that is to be duplicated
		 *
		 *\return	EntityPtr			Pointer to duplicate entity
		 */
		EntityPtr ECSManager::CloneEntity(EntityPtrRef other) {
			if(!other) { 
				ECS_error("Attempting to Clone entity from a nullptr!");	
				return EntityPtr();
			}
			EntityPtr cloneRoot = this->_cloneEntity(other);
			if(!cloneRoot) { 
				ECS_error("Entity Clone failed! This likely means we're out of memory or IDs!");
				return EntityPtr();
			}
			ECS_info("Generated a cloned Entity tree for Entity.ID %10d. Clone entity ID: %10d", other->getID().id, cloneRoot->getID().id);
			if(other->getParent()) {
				ECS_info("Attaching cloned Entity tree to Parent.ID %10d", other->getParent()->getID().id);
				other->getParent()->attachEntity(cloneRoot, other->getParent());
			}
			return cloneRoot;
		}

		//Recursively calls itself to clone entity tree
		EntityPtr ECSManager::_cloneEntity(EntityPtrRef other) {
			vector<EntityPtr> clonedChildren;
			vector<EntityPtr> eVec;
			EntityPtr cloned;
			if(other->getNumChildren() != 0) {
				ECS_info("Cloning %d child Entities attached to Entity.ID %10d", other->getNumChildren(), other->getID().id);
				other->getAllChildren(eVec);
				for(EntityPtr& pV : eVec) {
					if(pV) {
						cloned = _cloneEntity(pV);
						if(cloned) {
							ECS_info("Cloned child of Entity.ID %10d. Clone.ID: %10d", other->getID().id, cloned->getID().id);
							clonedChildren.push_back(cloned);
						} else {
							ECS_error("Failed child clone");
						}
					}
				}
			}
			EntityPtr clone = CreateEntity(other->getCTag());
			if(!clone) { 
				ECS_error("Failed to clone self entity!");
				return EntityPtr();
			}
			ECS_info("Cloning Entity components");
			ECGroupsItor oCompWalker = mEntityComponentGroups.find(other->getID());
			if(oCompWalker != mEntityComponentGroups.end()) {
				for(auto componentID : oCompWalker->second.mComponentSet){
					ECS_info("Creating clone of Component.ID %10d", componentID.id);
					CloneComponent(componentID, clone->getID());
				}
			}
			for(EntityPtr child : clonedChildren) {
				clone->attachEntity(child, clone);
			}
			//clone->mECS = this;
			return clone;
		}
		bool ECSManager::AttachEntity(const InstanceID toAttach, const InstanceID parentID) {
			auto toAttachFinder = mEntityIDMap.find(toAttach);
			auto parentFinder = mEntityIDMap.find(parentID);
			ECS_info("Attaching Entity.ID %10d to Parent.ID %10d", toAttach.id, parentID.id);
			if(toAttachFinder == mEntityIDMap.end() || parentFinder == mEntityIDMap.end() ||
				!toAttachFinder->second || !parentFinder->second) {
				ECS_info("Failed to find Parent.ID %10d to attach Entity.ID %10d to", parentID.id, toAttach.id);
				return false;
			}
			return parentFinder->second->attachEntity(toAttachFinder->second, parentFinder->second);
		}
	
		/*!
		 * This function will set an Entity's tag to the new value, moving it
		 * in it's internal lists to properly keep track of it
		 *\param	entityPointer	Pointer to entity to re-tag
		 *\param	newTag			New Entity tag to use
		 *
		 *\return	bool			A boolean value, true on succesful re-tagging, false on
		 *							any failure
		 */
		bool ECSManager::SetTag(EntityPtrRef entityPointer, const std::string& newTag) {
			if(!entityPointer || newTag.length() == 0) {
				if(!entityPointer) ECS_error("Attempting to SetTag on a nullptr!");
				if(newTag.length() == 0) ECS_error("Attempting to SetTag using an empty tag!");
				return false;
			}
			auto tagRange = mEntityTagMap.equal_range(entityPointer->getCTag());
			assert(tagRange.first != mEntityTagMap.end());
			ECS_info("Found Entity tag range for Entity.Tag %s", entityPointer->getCTag());
			ETiterator finder = tagRange.first;
			for(;finder != tagRange.second; ++finder) {
				if(finder->second && finder->second->getID() == entityPointer->getID()){
					ECS_info("Found Entity.ID %10d in Entity Tag map, reinserting with modified tag", entityPointer->getID().id);
					mEntityTagMap.erase(finder);
					break;
				}
				ECS_error("Could not find Entity.ID %10d in Entity Tag map %s", entityPointer->getID().id, entityPointer->getCTag().c_str());
				return false;
			}
			entityPointer->setTag(newTag);
			ETiterator newPlacer = mEntityTagMap.insert(make_pair(newTag, entityPointer));
			assert(newPlacer != mEntityTagMap.end());
			ECS_info("Reinserted Entity.ID %10d into Entity Tag map with Tag %s", entityPointer->getID().id, newTag.c_str());
			return true;
		}
		/*! This function will attempt to find an entity matching the specified tag
		 * returning the first one found that matches. Since insertion order is not preserved
		 * there is no guarantee that this function will return the first-inserted entity with the specified tag
		 *\param	entityTag		Entity tag to use for finding a matching Entity
		 *
		 *\return	EntityPtr			Pointer to a found entity, or nullptr on failure, or if no entity with the specified tag was found
		 */
		EntityPtr ECSManager::GetEntity(const std::string& entityTag) {
			if(entityTag.length() == 0) { 
				ECS_error("Cannot get Entity for an empty tag!");
				return EntityPtr();
			}
			ETiterator finder = mEntityTagMap.find(entityTag);
			if(finder == mEntityTagMap.end()) { 
				ECS_error("Cannot find Entity.Tag %s in Entity tag map", entityTag.c_str());
				return EntityPtr();
			}
			return finder->second;
		}

		 /*!
		  * This function will attempt to find an entity matching the specified entity id
		  * returning the pointer to it, or nullptr if such an Entity is not found
		  *\param	entityID		Entity id to use for finding a matching Entity
		  *
		  *\return	EntityPtr		Pointer to a found entity, or a nullptr on failure or if no entity with the specified id was found
		  */
		EntityPtr ECSManager::GetEntity(const InstanceID entityID) {
			EIiterator finder = mEntityIDMap.find(entityID);
			if(finder == mEntityIDMap.end()) { 
				ECS_error("Could not find Entity.ID %10d in Entity ID map", entityID.id);	
				return EntityPtr();
			}
			return finder->second;
		}
		/*! This function will attempt to find all entities matching the specified tag
		 * returning a STL vector containing pointers to each found Entity
		 * Since insertion order is not preserved, there is no guarantee that the returned vector has Entities
		 * in the order they were inserted in.
		 *\param	entityTag		Entity tag to use for finding a matching Entity
		 *
		 *\return	vector<EntityPtr>	Vector containing pointers to matching entities, or an empty vector if none were found, or there was an error
		 */
		void ECSManager::GetEntities(const std::string& entityTag, vector<EntityPtr>& out) {
			if(entityTag.length() == 0) { 
				ECS_error("Cannot get Entities for an empty Entity.Tag");
				return; // make_shared<vector<EntityPtr>>();
			}
			auto tagRange = mEntityTagMap.equal_range(entityTag);
			if(tagRange.first == mEntityTagMap.end()) { 
				ECS_warn("Could not find any Entities for Tag %s", entityTag.c_str());
				return;
			}
			ETiterator walker = tagRange.first;
			for(; walker != tagRange.second; ++walker) {
				if(walker->second) out.push_back(walker->second);
			}
		}
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
		bool ECSManager::DestroyEntity(EntityPtrRef toDestroy) {
			if(!toDestroy) { 
				ECS_error("Attempting to destroy Entity via a nullptr");	
				return false;
			}
			ECS_info("Destroying Entity.ID %10d, Entity.Tag %s", toDestroy->getID().id, toDestroy->getCTag().c_str());

			EntityPtr parent = toDestroy->getParent();
			if(parent)
				parent->detachChildEntity(toDestroy);
			return this->_destroyEntity(toDestroy);
		}
	
		bool ECSManager::_destroyEntity(EntityPtrRef toDestroy) {
			this->DestroyEntityComponents(toDestroy->getID());
			ECS_info("Getting children for Entity.ID %10d", toDestroy->getID().id);
			vector<EntityPtr> children;
			toDestroy->getAllChildren(children);
			for(EntityPtr pE : children) {
				if(pE) {
					_destroyEntity(pE);
				}
			}
			toDestroy->mChildren.clear();
			//Remove Components
			ECS_info("Getting attached components for Entity.ID %10d", toDestroy->getID().id);
			ECGroupsItor ec = mEntityComponentGroups.find(toDestroy->getID());
			mEntityComponentGroups.erase(ec);

			EIiterator eid = mEntityIDMap.find(toDestroy->getID());
			auto tagRange = mEntityTagMap.equal_range(toDestroy->getCTag());
			assert(eid != mEntityIDMap.end());
			assert(tagRange.first != mEntityTagMap.end());
			ECS_info("Found Entity in Entity ID and Entity Tag maps");
			for(ETiterator walker = tagRange.first; walker != tagRange.second; ++walker) {
				if(walker->second && walker->second->getID() == toDestroy->getID()){
					mEntityTagMap.erase(walker);
					break;
				}
				return false;
			}
			EntityDestroyEvent.Call(
				EntityEventArgs(toDestroy->getID(), toDestroy));
			mEntityIDMap.erase(eid);
			return true;
		}
	
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
		bool ECSManager::DestroyEntity(const InstanceID id) {
			EIiterator finder = mEntityIDMap.find(id);
			if(finder == mEntityIDMap.end() || !(finder->second)) { 
				if(finder == mEntityIDMap.end()) ECS_error("Could not find Entity.ID %10d in Entity ID map", id.id);
				if(!(finder->second)) ECS_error("Critical! Entity found for Entity.ID %10d but the pointer is null!", id.id);
				return false;
			}
			EntityPtr toDestroy = finder->second;
			EntityPtr parent = toDestroy->getParent();
			if(parent)
				parent->detachChildEntity(toDestroy);
			return this->_destroyEntity(toDestroy);
		}

		bool ECSManager::DestroyEntityComponents(const InstanceID entityID) {
			ECGroupsItor ec = mEntityComponentGroups.find(entityID);
			assert(ec != mEntityComponentGroups.end());

			_entityContainer& eset = ec->second;
			CIiterator idFinder;
			ECGroupsItor parenter;
			
			//Remove components
			for(auto componentID : eset.mComponentSet) {
				ECS_info("Found Component.ID %10d attached to Entity.ID %10d", componentID.id, entityID.id);
				idFinder = mComponentIDMap.find(componentID);
				assert(idFinder != mComponentIDMap.end());
				
				//detach from parent entity
				parenter = mEntityComponentGroups.find(entityID);
				assert(parenter != mEntityComponentGroups.end());
				ECS_info("Detaching Component.ID %10d from Entity.ID %10d", componentID.id, entityID.id);
				CTGroupsItor tagFinder = mComponentTagGroups.find(idFinder->second->mTypeID);
				assert(tagFinder != mComponentTagGroups.end());
				tagFinder->second.remove(componentID);

				idFinder->second->OnDestroy();
				auto comp = idFinder->second;
				ComponentDestroyEvent.Call(
					ComponentEventArgs(	comp->getID(), 
										comp->GetParentID(), 
										comp->mTypeID, 
										comp));
				mComponentIDMap.erase(idFinder);
			}
			eset.mComponentSet.clear();
			return true;
		}

		size_t ECSManager::GetNumComponents(const InstanceID entityID) {
			ECS_info("Getting number of components for Entity.ID %10d", entityID.id);
			if(entityID == error_iid) {
				return mComponentIDMap.size();
			} else {
				ECGroupsCItor counter = mEntityComponentGroups.find(entityID);
				if(counter == mEntityComponentGroups.end()) { 
					ECS_error("Could not find Entity.ID %10d in Entity component map!", entityID.id);
					return 0;
				}
				return counter->second.mComponentSet.size();
			}
		}

		ECSManager* ECSManager::GetECS() {
			auto pScene = SceneManager::get()->GetCurrentScene();
			if(pScene) {
				return pScene->getECS();
			}
			return nullptr;
		}

		//Updates
		void ECSManager::PreUpdate() {
			for(auto& CompPair : mComponentIDMap) {
				auto& CompPtr = CompPair.second;
				if(CompPtr && CompPtr->IsActive()) {
					CompPtr->PreUpdate();
				}
			}
		}
		void ECSManager::Update() {
			for(auto& CompPair : mComponentIDMap) {
				auto& CompPtr = CompPair.second;
					if(CompPtr && CompPtr->IsActive()) {
						CompPtr->Update();
					}
			}
		}
		void ECSManager::PostUpdate() {
			for(auto& CompPair : mComponentIDMap) {
				auto& CompPtr = CompPair.second;
				if(CompPtr && CompPtr->IsActive()) {
					CompPtr->PostUpdate();
				}
			}
		}

		//Entity Activity:
		void ECSManager::SetEntityActivity(const InstanceID& entityID, const bool activity) {
			ECS_info("Setting Entity.ID %10d to active", entityID.id);
			//Find the entity Component Group
			auto finder = mEntityComponentGroups.find(entityID);
			if(finder != mEntityComponentGroups.end()) {
				//Grab the actual group object
				auto componentGroup = finder->second;
				//For every component ID in the set...
				for(auto& comp : componentGroup.mComponentSet) {
					//Get the pointer to the component...
					auto compPtr = mComponentIDMap.find(comp);
					if(compPtr != mComponentIDMap.end()) {
						//And activate it
						compPtr->second->SetActive(activity);
					}
				}
			}
		}

		
		void ECSManager::dumpEntityToLog(const EntityPtr entity) {
			if(!entity) {
				ECS_debug("Null entity dumped to log");
				return;
			}
			auto egroup = mEntityComponentGroups.find(entity->getID());
			auto comps = egroup->second.mComponentSet;
			ECS_debug("Entity Dump\n\tEntity.ID: %10d\n\t Children: %10d", entity->getID().id, comps.size());

			for(auto c : comps) {
				auto comp = mComponentIDMap.find(c);
				if(comp != mComponentIDMap.end()) {
					auto cd = comp->second;
					ECS_debug("\tChild:\n\t\tComponent.ID: %10d\n\t\tComponent.TypeID: %10d\n\t\tComponent.Active: %c\n\t\tComponent.Flags: %x", 
						cd->getID().id, cd->mTypeID.typeID, BoolToChar(cd->IsActive()), cd->hardFlags); 
				}
			}
		}
		void ECSManager::dumpComponentToLog(const IComponentPtr component) {
			if(!component) {
				ECS_debug("Null component dumped to log");
				return;
			}
			ECS_debug("Component Dump\n\tComponent.ID: %10d\n\tComponent.TypeID: %10d\n\tComponent.Active: %c\n\tComponent.Flags: %x",
				component->getID().id, component->mTypeID.typeID, BoolToChar(component->IsActive()), component->hardFlags);
		}


		//This is odd to stick in here, but I dont want to include SceneManager in Component.cpp
		ECSManager* const Component::GetECS() const {
			return ECSManager::GetECS();
		}
		Camera* const Component::GetCamera() const {
			auto scene = SceneManager::GetCurrentScene();
			assert(scene);
			return scene->GetCamera();
		}
		Scene* const Component::GetScene() const{
			return SceneManager::GetCurrentScene();
		}
}
