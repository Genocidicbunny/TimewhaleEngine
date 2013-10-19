#include "TransformComponent.h"
#include "SpriteComponent.h"
#include "Scene.h"
#include "Camera.h"
#include "PlatformManager.h"
#include <functional>
#include <algorithm>

namespace Timewhale {

	Scene::Scene()
		:mInitializerFunc(nullptr),
		mID(GetNextReservedID()),
		mName(""),
		ECS(new ECSManager(mID)),
		mStatus(),
		mCamera(new Camera())
	{
		auto plat = PlatformManager::get();
		if(plat) mCamera->SetBounds((float)plat->width(), (float)plat->height());
	}

	Scene::Scene(const SceneInitFunc& initFunc) 
		:mInitializerFunc(initFunc),
		mID(GetNextReservedID()),
		mName(""),
		ECS(new ECSManager(mID)),
		mStatus(true, true),
		mCamera(new Camera())
	{
		auto plat = PlatformManager::get();
		if(plat) mCamera->SetBounds((float)plat->width(), (float)plat->height());
	}
	Scene::Scene(const SceneInitFunc& initFunc, const std::string& name)
		:mInitializerFunc(initFunc),
		mID(GetNextReservedID()),
		mName(name),
		ECS(new ECSManager(mID)),
		mStatus(true, true),
		mCamera(new Camera())
	{
		auto plat = PlatformManager::get();
		if(plat) mCamera->SetBounds((float)plat->width(), (float)plat->height());
	}
	Scene::~Scene() {
		auto ecs = ECS.release();
		if(ecs) delete ecs;
		//The delete also deleted our event which deletes all of the handlers from it
	}

	void Scene::Init() {
		//Set up event handlers
		componentHandlers[0] = ECS->ComponentCreateEvent += 
			[this](const ComponentEventArgs& args) {
				this->HandleComponentCreated(args);
			};
		componentHandlers[1] = ECS->ComponentDestroyEvent += 
			[this](const ComponentEventArgs& args) {
				this->HandleComponentDestroyed(args);
			};
		entityHandlers[0] = ECS->EntityCreateEvent += 
			[this](const EntityEventArgs& args) {
				this->HandleEntityCreated(args);
			};
		entityHandlers[1] = ECS->EntityDestroyEvent += 
			[this](const EntityEventArgs& args) {
				this->HandleEntityDestroyed(args);
			};
		if(mInitializerFunc) {
			mInitializerFunc(ECS.get());
		}
	}
	ECSManager* Scene::getECS() const {
		return ECS.get();
	}
	const SceneID Scene::getSceneID() const {
		return mID;
	}
	std::string Scene::getSceneName() const {
		return mName;
	}

	bool Scene::IsUpdating() {
		return mStatus.SceneUpdating;
	}
	bool Scene::IsRendering() {
		return mStatus.SceneRendering;
	}
	void Scene::SetUpdating(const bool u_status) {
		mStatus.SceneUpdating = u_status;
	}
	void Scene::SetRendering(const bool r_status) {
		mStatus.SceneRendering = r_status;
	}

	void Scene::HandleComponentCreated(const ComponentEventArgs& args) {
		assert(args.componentID);
		assert(args.componentParentID);
		assert(args.componentType);
		assert(args.componentPtr);

		
		Scene_info("HandlerComponentCreated called:\n\tComponent.ID: %10d Parent.ID: %10d, Component.Type: %10d", 
			args.componentID.id, args.componentParentID.id, args.componentType.typeID);

		//The search is in the if's because we dont want to look things up
		//unless we really need to, which is only if the components are of the right type

		if(args.componentType == TransformTypeID) {
			auto finder = mOSB.find(args.componentParentID);
			if(finder == mOSB.end()) {
				Scene_warn("Attempting to add a component to a non-existing OSB. Cannot continue. ID: %10d, Parent.ID: %10d", args.componentID.id, args.componentParentID.id);
				return;
			}
			auto osb = finder->second;
			osb->transformPtr = args.componentPtr;
		} else if(args.componentType == SpriteTypeID) {
			auto finder = mOSB.find(args.componentParentID);
			if(finder == mOSB.end()) {
				Scene_warn("Attempting to add a component to a non-existing OSB. Cannot continue. ID: %10d, Parent.ID: %10d", args.componentID.id, args.componentParentID.id);
				return;
			}
			auto osb = finder->second;
			if(!osb->transformPtr) {
				Scene_warn("Attempting to add a SpriteComponent to a OSB without a Transform!");
			}
			osb->spritePtrs.push_back(args.componentPtr);
		}
		//If it's any other component, we don't care
	}
	void Scene::HandleComponentDestroyed(const ComponentEventArgs& args) {
		assert(args.componentID);
		assert(args.componentParentID);
		assert(args.componentType);
		assert(args.componentPtr);

		Scene_info("HandlerComponentDestroyed called:\n\tComponent.ID: %10d Parent.ID: %10d, Component.Type: %10d", 
			args.componentID.id, args.componentParentID.id, args.componentType.typeID);

		if(args.componentType == TransformTypeID) {
			auto finder = mOSB.find(args.componentParentID);
			if(finder == mOSB.end()) {
				Scene_warn("Attempting to remove a component in a non-existing OSB! Cannot continue. ID: %10d, Parent.ID: %10d", args.componentID.id, args.componentParentID.id);
				return;
			}
			auto osb = finder->second;
			osb->transformPtr = nullptr;
		} else if(args.componentType == SpriteTypeID) {
			auto finder = mOSB.find(args.componentParentID);
			if(finder == mOSB.end()) {
				Scene_warn("Attempting to remove a component in a non-existing OSB! Cannot continue. ID: %10d, Parent.ID: %10d", args.componentID.id, args.componentParentID.id);
				return;
			}
			auto osb = finder->second;
			auto spriteFinder = std::find(osb->spritePtrs.begin(), osb->spritePtrs.end(), args.componentPtr);
			if(spriteFinder == osb->spritePtrs.end()) {
				Scene_error("Attempting to remove a non-existing sprite from OSB!");
				return;
			}
			osb->spritePtrs.erase(spriteFinder);
		}
		//any other component we don't care.
	}
	void Scene::HandleEntityCreated(const EntityEventArgs& args) {
		assert(args.entityID);
		assert(args.entityPtr);
		
		Scene_info("HandleEntityCreated called:\n\tEntity.ID: %10d", args.entityID.id);

		//Make sure we dont already have an OSB for this..would be silly if we did
		auto finder = mOSB.find(args.entityID);
		if(finder != mOSB.end()) {
			Scene_error("Attempting to add a new OSB for an existing entity! ID: %10d", args.entityID.id);
			return;
		}

		auto res = mOSB.emplace(std::make_pair(args.entityID, make_shared<OSB>(args.entityID, args.entityPtr)));
		if(!res.second ) {
			Scene_error("Error creating new OSB for Entity.ID %10d", args.entityID.id);
			return;
		}

		//we've emplaced a new OSB for this entity, that's it!
	}
	void Scene::HandleEntityDestroyed(const EntityEventArgs& args) {
		assert(args.entityID);
		assert(args.entityPtr);

		Scene_info("HandleEntityDestroyed called:\n\tEntity.ID: %10d", args.entityID.id);

		auto finder = mOSB.find(args.entityID);
		if(finder == mOSB.end()) {
			Scene_error("Attempting to delete a non-existent OSB! ID: %10d", args.entityID.id);
			return;
		}

		mOSB.erase(finder);
		return;
	}

	void Scene::PreUpdate() {
		if(ECS) ECS->PreUpdate();
	}
	void Scene::Update() {
		if(ECS) ECS->Update();
	}
	void Scene::PostUpdate() {
		if(ECS) ECS->PostUpdate();
	}

	size_t Scene::SubmitSceneForRender(SceneRenderData& rdata_out) {
		//Clear out old data
		//for(auto& linfo : rdata_out.layers) {
		//	linfo.NumElements = 0;
		//	linfo.rbuf_vec.clear();
		//}

		//dont clear out old data
		//record camera info:
		rdata_out.SceneCamera = GetCamera();

		//go through all osbs
		size_t num_osb = 0;

		for(auto& osb : mOSB) {
			auto osbptr = osb.second;
			if(!osbptr) continue;

			auto transform = dynamic_pointer_cast<Transform2D>(osbptr->transformPtr);
			for(auto& spriteI : osbptr->spritePtrs) {
				auto sprite = dynamic_pointer_cast<SpriteComponent>(spriteI);
				if(!sprite || !sprite->IsVisible() || !transform) continue;


				if(num_osb >= rdata_out.rbuf_vec.size()) {
					rdata_out.rbuf_vec.resize(num_osb + 10);
				}
				auto& bounds = transform->GetBoundsRect();
				rdata_out.rbuf_vec[num_osb] = 
					std::move(RenderBuffer(
						bounds.x, bounds.y, 
						bounds.width, bounds.height, 
						transform->GetRotation(),
						sprite->GetLayer(),
						sprite->GetSpriteSheetID(), 
						sprite->GetCurrentFrame(),
						sprite->GetHorizontalMaxFrames(), sprite->GetVerticalMaxFrames(),
						sprite->GetR(), sprite->GetG(), sprite->GetB()));
				++num_osb;

			}
		}

		//sort the render buffers
		std::stable_sort(rdata_out.rbuf_vec.begin(), rdata_out.rbuf_vec.begin() + num_osb, 
			[](const RenderBuffer& lhs, const RenderBuffer& rhs) {
				return lhs.layer < rhs.layer;
			});

		return num_osb;
		//rdata_out.rbuf_vec.clear();
		//rBufLess bufferComp;

		//for(auto osb : mOSB) {
		//	auto osbptr = osb.second;
		//	if(!osbptr) continue;

		//	//cast the pointers in the osb
		//	//auto sprite = dynamic_pointer_cast<SpriteComponent>(osbptr->spritePtr);
		//	auto transform = dynamic_pointer_cast<Transform2D>(osbptr->transformPtr);
		//	for(auto& spriteI : osbptr->spritePtrs) {
		//		auto sprite = dynamic_pointer_cast<SpriteComponent>(spriteI);
		//		if(!sprite || !sprite->IsVisible() || !transform) 
  //                  continue;
		//		//auto& linfo = rdata_out.layers[*sprite->GetLayer()];

		//		//occlusion checking will occur here

		//		//Push osb into buffer
		//		//linfo.NumElements++;
		//		auto& bounds = transform->GetBoundsRect();
		//		rdata_out.rbuf_vec.emplace_back(
		//			RenderBuffer(	bounds.x, bounds.y, 
		//							bounds.width, bounds.height, 
		//							transform->GetRotation(),
  //                                  sprite->GetLayer(),
		//							sprite->GetSpriteSheetID(), 
		//							sprite->GetCurrentFrame(),
		//							sprite->GetHorizontalMaxFrames(), sprite->GetVerticalMaxFrames(),
		//							sprite->GetR(), sprite->GetG(), sprite->GetB()));
  //              push_heap<vector<RenderBuffer>::iterator, rBufLess>(
  //                  rdata_out.rbuf_vec.begin(), 
  //                  rdata_out.rbuf_vec.end(), 
  //                  bufferComp);
		//	}
		//}
  //      sort_heap<vector<RenderBuffer>::iterator, rBufLess>(
  //          rdata_out.rbuf_vec.begin(), 
  //          rdata_out.rbuf_vec.end(), 
  //          bufferComp);
	}

	Camera* const Scene::GetCamera() const {
		return mCamera;
	}
	void Scene::SetCamera(Camera* cam) {
		if(cam) mCamera = cam;
	}

}