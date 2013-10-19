
#include "SceneManager.h"
#include "SceneTable.h"
#include "RenderSystem.h"
#include "TWLogger.h"

using namespace std;
using namespace Timewhale;

SceneManagerPtr SceneManager::sManager = nullptr;



SceneManager::SceneManager() 
    : currentScene(nullptr),
	  changingScene(nullptr),
	  mChanging(false)
{}

SceneManager::~SceneManager() {
    if(currentScene) delete currentScene;
	if(changingScene) delete changingScene;
}

SceneManagerPtr const SceneManager::Create() {
    if (!sManager)
        sManager = make_shared<SceneManager>();
    return sManager;
}

//Scene* const SceneManager::getSceneByID(const SceneID& id) {
//    if (sManager->mSceneIDMap.count(id) < 1)
//        return nullptr;
//
//    return sManager->mSceneIDMap.at(id);
//}
Scene* const SceneManager::GetCurrentScene() {
	return get()->currentScene;
}

bool SceneManager::Init() {
    return true;
}

void SceneManager::Update() {
	////Changing scenes pre-empts any pops/pushes
	//if(mChanging) {
	//	if(changingScene.length() == 0) {
	//		log_sxwarn("SceneManager", "Error, attempting to change to a scene with an empty name. Change being ignored");
	//		return;
	//	}
	//	log_sxinfo("SceneManager", "Attempting to change to scene \"%s\"", changingScene.c_str());
	//	mChanging = false;

	//	auto _scene_map = GetSceneInfoMap();
	//	auto finder = _scene_map.find(changingScene);
	//	SceneInitf init_func(nullptr);
	//	if(finder == _scene_map.end()) {
	//		log_sxwarn("SceneManager", "Error, could not find the info for scene \"%s\", an empty Scene will be generated", changingScene.c_str());
	//	} else {
	//		init_func = finder->second->initializer_func;
	//	}
	//	//clear the current scene stack
	//	size_t currentStackSize = mSceneList.size();
	//	for(auto scene : mSceneList) {
	//		if(scene) {
	//			mSceneIDMap.erase(scene->getSceneID());
	//			delete scene;
	//		}
	//	}
	//	mSceneList.clear();
	//	mPopping = 0;
	//	auto newScene = new Scene(init_func, changingScene);
	//	if(newScene) {
	//		newScene->Init();
	//		newScene->SetUpdating(true);
	//		newScene->SetRendering(true);
	//		
	//	}

	//}
 //   if (mPopping > 0) {
 //       for (; mPopping > 0; --mPopping) {
 //           Scene* scene = mSceneList.front();
 //           mSceneList.pop_front();
 //           if(scene) {
	//			mSceneIDMap.erase(scene->getSceneID());
	//			delete scene;
	//		}
 //           if (mSceneList.empty())
 //               break;
 //       }
 //       mPopping = 0;
 //   }

 //   if (mTransitioning && mNextScenes.size() > 0) {
	//	//Moved the manipulations inside a check for null. If for some reason newScene is null
	//	//This wont cause everything to blow up
	//	auto newScene = mNextScenes.front();
	//	if(newScene) {
	//		mSceneList.push_front(newScene);
	//		mNextScenes.pop_front();
	//		newScene->Init();
	//		newScene->SetUpdating(true);
	//		newScene->SetRendering(true);
	//	} else {
	//		log_sxerror("SceneManager", "Error, NextScenes returned a nullptr!");
	//	}

 //       if (mNextScenes.empty())
 //           mTransitioning = false;
 //   }

 //  /* auto itor = mSceneList.begin();
 //   auto end = mSceneList.end();*/
	//auto count = mSceneList.size();

	//If we're expecting to change
	if(mChanging) {
		if(!changingScene) {
			log_sxerror("SceneManager", "!!! Critical Error! Changing to a null Scene ptr! Something went very wrong !!!");
			mChanging = false;
			return;
		}
		if(currentScene) {
			delete currentScene;
		}
		currentScene = changingScene;
		changingScene = nullptr;
		currentScene->Init();
		mChanging = false;
	}

	//Gonna rework this bit later now that we've only got one scene to worry about
	/*renderData.reserve(1);
	renderData.clear();*/

	RunScenePreUpdates();
	RunSceneUpdates();
	SubmitForRendering();
	RunScenePostUpdates();
}

void SceneManager::Shutdown() {
    //mScenes.clear();
    //mSceneIDMap.clear();
	
    //auto itor = mSceneList.begin();
    //auto end = mSceneList.end();

    //for (; itor != end; ++itor) {
    //    if (*itor)
    //    delete (*itor);
    //    *itor = nullptr;
    //}
    //mSceneList.clear();
	
	if(changingScene) delete changingScene;
	if(currentScene) delete currentScene;
	changingScene = nullptr;
	currentScene = nullptr;
	sManager.reset();
}

bool SceneManager::_CreateScene(
    const std::string &name, 
    const SceneInitf &initFunc) 
{
	const SceneInitf& initf = initFunc;

    SceneInfoMap sceneTable = GetSceneInfoMap();

    if (sceneTable.count(name) > 0) return true;
    RegisterSceneInfo(name, "", initFunc);
    return true;
}

//void SceneManager::push(const std::string &name) {
//    //SceneInfoMap sceneTable = GetSceneInfoMap();
//    //if (sceneTable.count(name) < 0) return;
//    //auto sceneInfo = sceneTable.at(name);
//
//    //Scene* scene = new Scene(sceneInfo->initializer_func, sceneInfo->scene_name);
//
//    //mScenes.insert(make_pair(name, scene));
//    //mSceneIDMap.insert(make_pair(scene->getSceneID(), scene));
//
//    //mNextScenes.push_back(scene);
//    //mTransitioning = true;
//	changeScene(name);
//}
void SceneManager::_ChangeScene(const std::string& name) {
	auto sceneTable = GetSceneInfoMap();
	auto finder = sceneTable.find(name);
	if(finder == sceneTable.end()) {
		log_sxerror("SceneManager", "Error changing to Scene \"%s\"!", name.c_str());
		return;
	}
	if(changingScene) delete changingScene;
	changingScene = new Scene(finder->second->initializer_func, name);
	mChanging = true;
}
//
//void SceneManager::pop() {
//    mPopping++;
//}
//
//Scene* const SceneManager::current() {
//    if (mSceneList.size() > 0)
//        return mSceneList.front();
//    else return nullptr;
//}
//
//Scene* const SceneManager::getScene(const std::string &name) {
//    if (mScenes.count(name) < 1) return nullptr;
//
//    return mScenes.at(name);
//}

//bool SceneManager::empty() {
//    return mSceneList.empty();
//}
//
//int SceneManager::size() {
//    return mSceneList.size();
//}
//
//bool SceneManager::onStack(const std::string &name) {
//	for(auto scenePtr : mSceneList) {
//		if(scenePtr && scenePtr->getSceneName() == name) return true;
//	}
//	return false;
//}


void SceneManager::RunScenePreUpdates() {
	/*for(auto scene : mSceneList) {
		if(scene && scene->IsUpdating()) scene->PreUpdate();
	}*/
	if(currentScene && currentScene->IsUpdating())
		currentScene->PreUpdate();
}

void SceneManager::RunSceneUpdates() {
	/*for(auto scene : mSceneList) {
		if(scene && scene->IsUpdating()) scene->Update();
	}*/
	if(currentScene && currentScene->IsUpdating())
		currentScene->Update();
}

void SceneManager::RunScenePostUpdates() {
	//for(auto scene : mSceneList) {
	//	if(scene && scene->IsUpdating()) scene->PostUpdate();
	//}
	if(currentScene && currentScene->IsUpdating())
		currentScene->PostUpdate();
}

void SceneManager::SubmitForRendering() {
	//Rendering stuff goes here
	/*size_t sceneRelativeCounter = 0;
	for(auto scenePtr : mSceneList) {
		if(scenePtr && scenePtr->IsRendering()) {
			renderData.emplace_back(sceneRelativeCounter++);
			auto last_scene_data = &renderData.back();
			scenePtr->SubmitSceneForRender(*last_scene_data);
			int x = 0;
		}
	}*/

    //auto itor = mSceneList.rbegin();
    //for (; itor != mSceneList.rend(); ++itor) {
    //    if ((*itor) && (*itor)->IsRendering()) {
    //        SceneRenderData sceneData;
    //        (*itor)->SubmitSceneForRender(sceneData);
    //        renderData.push_back(sceneData);
    //    }
    //}
	//SceneRenderData sceneData;
	if(!currentScene || !currentScene->IsRendering()) return;
	auto num = currentScene->SubmitSceneForRender(render_data);
	//renderData.push_back(sceneData);

	auto renderer = RenderSystem::get();
	assert(renderer);
	renderer->BufferScenes(render_data, num);
	//Swap with this when I get a chance:
	//renderer->BufferScene(sceneData);
}


