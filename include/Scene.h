#pragma once
#ifndef __TIMEWHALE_SCENE_H_
#define __TIMEWHALE_SCENE_H_

#include <functional>
#include <memory>
#include <vector>
#include <unordered_map>
#include "InstanceID.h"
#include "TWLogger.h"
#include "ECS.h"
#include "Rocket\Core.h"
#include "OSB.h"

#define Scene_info(format, ...) log_sxinfo("Scene", format, ##__VA_ARGS__)
#define Scene_warn(format, ...) log_sxwarn("Scene", format, ##__VA_ARGS__)
#define Scene_error(format, ...) log_sxerror("Scene", format, ##__VA_ARGS__)
#define Scene_debug(format, ...) log_sxdebug("Scene", format, ##__VA_ARGS__)


namespace Timewhale {
	class ECSManager;
	class Camera;
//	class SceneManager;
	using std::function;
	using std::vector;
	using std::unique_ptr;
	using Timewhale::SceneID;
	typedef std::function<void (ECSManager*)> SceneInitFunc;

	class Scene {
		friend class SceneManager;
		typedef std::unordered_map<InstanceID, OSBPtr> OSBMap;
	private:
		struct SceneState {
			bool SceneUpdating;
			bool SceneRendering;
			SceneState(const bool _u, const bool _r)
				:SceneUpdating(_u),
				SceneRendering(_r)
			{}
			SceneState()
				:SceneUpdating(false),
				SceneRendering(false)
			{}
		} mStatus;
		//Scene-specific data
		SceneID mID;
		OSBMap mOSB;
		std::string mName;

		function<void (ECSManager*)> mInitializerFunc;
		unique_ptr<ECSManager> ECS;
		//Scene OSB storage area.
		EventHandler<ComponentEventArgs> componentHandlers[2];
		EventHandler<EntityEventArgs> entityHandlers[2];

		//Camera stuff
		Camera* mCamera;
	public:
		const SceneID getSceneID() const;
		ECSManager* getECS() const;
		std::string getSceneName() const;

		//Scene Status functions
		bool IsUpdating();
		bool IsRendering();
		void SetUpdating(const bool u_status);
		void SetRendering(const bool r_status);

		Scene(const SceneInitFunc& initFunc, const std::string& name);
        Scene(const SceneInitFunc &initFunc);
		Scene();
		~Scene();
		void Init();
		void HandleComponentCreated(const ComponentEventArgs& args);
		void HandleComponentDestroyed(const ComponentEventArgs& args);
		void HandleEntityCreated(const EntityEventArgs& args);
		void HandleEntityDestroyed(const EntityEventArgs& args);

		void PreUpdate();
		void Update();
		void PostUpdate();

		size_t SubmitSceneForRender(SceneRenderData& rdata_out);

		Camera* const GetCamera() const;
		void SetCamera(Camera* cam); 
	};

}


#endif