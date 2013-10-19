/*
 *
 * Scene Manager class
 *
*/
#pragma once
#ifndef __TIMEWHALE_SCENEMANAGER_H_
#define __TIMEWHALE_SCENEMANAGER_H_

#include <unordered_map>
#include <unordered_set>
#include <list>
#include <string>
#include <memory>

#include "Scene.h"
#include "SceneTable.h"
#include "InstanceID.h"

namespace Timewhale {

	class SceneManager {
		friend class TimewhaleEngine;
		friend class std::shared_ptr<SceneManager>;
		friend class std::_Ref_count_obj<SceneManager>;
  //      typedef std::unordered_map<std::string, Scene*> SceneMap;
  //      typedef std::unordered_map<SceneID, Scene*> SceneIDMap;
  //      typedef std::list<Scene*> SceneList;
		typedef std::vector<SceneRenderData> RenderDataVec;


        //SceneMap mScenes;
        //SceneIDMap mSceneIDMap;
        //SceneList mSceneList;
        //SceneList mNextScenes;
        //bool mTransitioning;
        //int mPopping;

		Scene* currentScene;;
		Scene* changingScene;
		bool mChanging;

		SceneRenderData render_data;

        static std::shared_ptr<SceneManager> sManager;

        SceneManager();
		void RunScenePreUpdates();
		void RunSceneUpdates();
		void RunScenePostUpdates();
		void SubmitForRendering();
	    static std::shared_ptr<SceneManager> const Create();
		bool Init();
		void Update();
		void Shutdown();
		bool _CreateScene(const std::string &name, const SceneInitf &initFunc = nullptr);
		void _ChangeScene(const std::string &name);
	public:

		inline static std::shared_ptr<SceneManager> const get() {
			if(!sManager) {
				log_sxerror("SceneManager", "Scene Manager has not been initialized!");
				assert(false);
			}
			return sManager;
        }

		static Scene* const GetCurrentScene();

		static bool CreateScene(
			const std::string &name, 
			const SceneInitf &initFunc = nullptr) 
		{
			auto sm = get();
			assert(sm);
			return sm->_CreateScene(name, initFunc);
		}

		static void ChangeScene(const std::string& name) {
			auto sm = get();
			assert(sm);
			sm->_ChangeScene(name);
		}
        //void push(const std::string &name);

        //void pop();

        //Scene* const current();

        //Scene* const getScene(const std::string &name);

        //bool empty();

        //int size();

        //bool onStack(const std::string &name);

        ~SceneManager();
	};

    typedef std::shared_ptr<SceneManager> SceneManagerPtr;
}

#endif