#pragma once
#ifndef __TIMEWHALE_CREG
#define __TIMEWHALE_CREG

#include <bitset>
#include <unordered_map>
#include <type_traits>
#include <typeindex>
#include <typeinfo>

#include "StringUtils.h"
#include "InstanceID.h"
//Component Registration stuff
#define CREG_FLAGS_SIZE			8
#define CREG_FLAGS_EMPTY		0x00
#define CREG_FLAGS_ENGCOMP		0x01
#define CREG_FLAGS_USERCOMP		0x02
#define CREG_FLAGS_CACHECOMP	0x04

namespace TImewhale {
	enum {
		FlagsEmpty = 0x00,
		FlagsEngComp = 0x01,
		FlagsUserComp = 0x02,
		FlagsCacheComp = 0x04,
		FlagsUniqueComp = 0x08
	};
}



/*!
 * This is the component registration macro
 * This macro accepts two parameters, the type and a string which is 
 * the component tag. This macro does not check for correct typing, 
 * although it will likely break if you pass bad parameters.
 * This macro uses several classes and templates to create a mapping
 * of strings to constructor function objects at compile time. This is why 
 * the macro should only be included once per component, and has to be
 * called to register a component. If this registration step is not performed
 * the component will not be found in the mapping, and therefore it will not
 * be possible to create a component of that type
 */
#define REGISTER_COMPONENT(TYPE, NAME)								\
	namespace {														\
		static_assert(std::is_base_of<Timewhale::IComponent, TYPE>::value, "" STR(TYPE) " does not derive from IComponent and cannot be registered as a Component! " __FILE__":"STR(__LINE__)); \
		template<class T>											\
		class ComponentRegistration;								\
		template<>													\
		class ComponentRegistration<TYPE> {							\
			static const CRegEntry<TYPE>& reg;						\
		};															\
		const CRegEntry<TYPE>& ComponentRegistration<TYPE>::reg =	\
		CRegEntry<TYPE>::Instance(CREG_FLAGS_USERCOMP);		\
	};																

#define REGISTER_COMPONENT_WFLAGS(TYPE, NAME, FLAGS) \
	namespace { \
		static_assert(std::is_base_of<Timewhale::IComponent, TYPE>::value, "" STR(TYPE) " does not derive from IComponent and cannot be registered as a Component! " __FILE__":"STR(__LINE__)); \
		template<class T> \
		class ComponentRegistration; \
		template<> \
		class ComponentRegistration<TYPE> { \
			static const CRegEntry<TYPE>& reg; \
		}; \
		const CRegEntry<TYPE>& ComponentRegistration<TYPE>::reg = \
		CRegEntry<TYPE>::Instance(FLAGS | CREG_FLAGS_CACHECOMP); \
	};
#define REGISTER_DEPENDENCY(_Dependent, _Dependency) \
	namespace { \
		static_assert(!std::is_same<_Dependent, _Dependency>::value, "A Component cannot depend on itself! ( " #_Dependent  " is marked as depending on itself - " __FILE__ ":"STR(__LINE__)")"); \
		static_assert(std::is_base_of<Timewhale::IComponent, _Dependent>::value, "Can only register a dependency for a class deriving from IComponent! " __FILE__ ":" STR(__LINE__) ); \
		static_assert(std::is_base_of<Timewhale::IComponent, _Dependency>::value, "Can only register a dependency that derives from IComponent! " __FILE__ ":" STR(__LINE__)); \
		template<typename T1, typename T2> \
		class CompDep; \
		template<> \
		class CompDep<_Dependent, _Dependency> { \
			static const DepPair<_Dependent, _Dependency>& dep; \
		}; \
		const DepPair<_Dependent, _Dependency>& CompDep<_Dependent, _Dependency>::dep = \
			DepPair<_Dependent, _Dependency>::Instance(); \
		};


/*!
 * This is the actual registration and creation of a component
 * This code can be used via the following:
 *
 *		First, the component must be a registered type:
 *			REGISTER_COMPONENT(FooComponent, "FooComponent");
 *
 *		Next, you must obtain an instance of the component registry map:
 *			Timewhale::ECS::CRegMap& reg = Timewhale::ECS::GetComponentRegMap();
 *
 *		After doing so, you can search through the registry for a registry matching
 *		the tag you provided, as such:
 *			Timewhale::ECS::CRegMap::iterator it = reg.find(name);
 *
 *		This will give you an iterator to the <k,v> matching the provided tag
 *		You should check that the returned iterator doesn't match the end of the map
 *		as that means there's no creation function
 *
 *		The final step is to actually call the function (or constructor rather), which will
 *		return a pointer to a Component of the specified type. This is done by obtaining the
 *		second member of the returned pair, and applying the () operator to it. This is in effect a call
 *		to the CreateComponent() function which is a wrapper around the default constructor call for a component.
 *
 *		Because the returned component is created using it's default constructor, the component's tag does not get
 *		set to the correct value. In fact, it should be set to MissingTag
 *		Note that this behaviour will likely change in the near future. It should be possible to use
 *		the registry tag as a parameter to the component's constructor. This would require that every
 *		component derived from Component have a constructor that accepts a string, which may not be needed
 *		for very simple components. As such it's possible there will be in fact two registration macros
 *		which will differentiate between the two constructor types.
 */
//namespace Timewhale {
//namespace ECS {

	typedef std::shared_ptr<Timewhale::IComponent> (*CreateComponentFunc)();
	typedef std::unordered_map<Timewhale::TypeID, CreateComponentFunc> CRegMap;
	typedef std::unordered_map<Timewhale::TypeID, std::bitset<CREG_FLAGS_SIZE>> CRegFlagMap;

	inline CRegMap& GetComponentRegMap() {
		static CRegMap reg;
		return reg;
	}
	inline CRegFlagMap& GetComponentFlagMap() {
		static CRegFlagMap freg;
		return freg;
	}


	//This is the actual function that should be called
	template<class T>
	std::shared_ptr<Timewhale::IComponent> CreateComponent() {
		return std::make_shared<T>();
	}

	//This is the registration struct
	//that gets created for every registered component
	//This is fairly cheap though
	template<class T>
	struct CRegEntry {
		//typedef std::shared_ptr<T> (*CreateComponentFunc)();
	public:
		static CRegEntry<T>& Instance(long long registry_flags) {
			static CRegEntry<T> instance(ctid<T>(), registry_flags);
			return instance;
		}
	private:
		CRegEntry<T>(const Timewhale::TypeID name, long long registry_flags) {
			CRegMap& map = GetComponentRegMap();
			CreateComponentFunc func = CreateComponent<T>;

			std::pair<CRegMap::iterator, bool> ret =
				map.insert(CRegMap::value_type(name, func));

			if(!ret.second) {
				///This is an error so we'll have to deal with it somehow
				//For now this is a crash
				//exit(-1);
				assert(false);
			}

			CRegFlagMap& fmap = GetComponentFlagMap();
			fmap.insert(CRegFlagMap::value_type(name, std::bitset<CREG_FLAGS_SIZE>(registry_flags)));
		}

		//Don't want people making copies of component registry entries
		//virtual CRegEntry(const CRegEntry&) = delete;
		CRegEntry(const CRegEntry<T>&) {};
		//virtual CRegEntry<T>& operator=(const CRegEntry<T>&) = delete;
	};

	struct component_deps_list {
		std::vector<Timewhale::TypeID> deps;
		void addDep(Timewhale::TypeID dep) { deps.push_back(dep); }
	};
	typedef component_deps_list CDepList;
	typedef std::unordered_map<Timewhale::TypeID, component_deps_list> ComponentDepsMap;

	inline ComponentDepsMap& GetDepsMap() {
		static ComponentDepsMap instance;
		return instance;
	}

	template<typename _C, typename _dep>
	struct DepPair {
		static DepPair<_C, _dep>& Instance() {
			static DepPair<_C, _dep> instance;
			return instance;
		}
	private:
		DepPair<_C, _dep>() {
			ComponentDepsMap& cdMap = GetDepsMap();
			ComponentDepsMap::iterator cdIt = cdMap.find(ctid<_C>());
			if(cdIt == cdMap.end()) {
				std::pair<ComponentDepsMap::iterator, bool> res = cdMap.insert(ComponentDepsMap::value_type(ctid<_C>(), component_deps_list()));
				assert(res.second);
				res.first->second.addDep(ctid<_dep>());
			} else {
				cdIt->second.addDep(ctid<_dep>());
			}

		}
	};
//}
//}
#endif

