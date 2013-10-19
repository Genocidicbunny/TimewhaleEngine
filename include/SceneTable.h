#pragma once
#ifndef __TIMEWHALE_SCENETABLE_H_
#define __TIMEWHALE_SCENETABLE_H_

#include <cassert>
#include <string>
#include <functional>
#include <memory>
#include <unordered_map>

#include "StringUtils.h"

namespace Timewhale {
	class ECSManager;
	typedef std::function<void (ECSManager*)> SceneInitf;

	struct scene_creation_info {
		std::string scene_name;
		std::string scene_blub;
		SceneInitf initializer_func;

		scene_creation_info(const std::string& name, const std::string& blub, const SceneInitf& init) 
			:scene_name(name),
			scene_blub(blub),
			initializer_func(init)
		{}
	};

	typedef std::shared_ptr<scene_creation_info> SceneInfoPtr;
	typedef std::unordered_map<std::string, SceneInfoPtr> SceneInfoMap;

	inline SceneInfoMap& GetSceneInfoMap() {
		static SceneInfoMap sreg;
		return sreg;
	}

	static void RegisterSceneInfo(const std::string& name, const std::string& blub, const SceneInitf& init) {
		SceneInfoMap& smap = GetSceneInfoMap();
		assert(smap.count(name) == 0);
		smap.emplace(make_pair(
			name, 
			std::make_shared<scene_creation_info>(name, blub, init)
		));
	}
}		 

#define CREATE_SCENE(NAME, INIT_FUNC) \
	namespace { \
		static const struct scene_##NAME { \
			scene_##NAME() { \
				RegisterSceneInfo(STR(NAME), "", INIT_FUNC ); \
			} \
		} scene_##NAME_inst;\
	} 
#define CREATE_SCENE_BLUB(NAME, BLUB, INIT_FUNC) \
	namespace { \
		static const struct scene_##NAME { \
			scene_##NAME() { \
				RegisterSceneInfo(STR(NAME), STR(BLUB), INIT_FUNC); \
			} \
		}scene_##NAME_inst; \
	}

#define SCENE_INIT_FUNC(CODE) \
	[](ECSManager* ECS) { \
		CODE ; \
	}
#endif