/*
 *Instance ID utility
 *Will assign sequential IDs as they are requrested.
 *The great thing about InstanceIDs is that they are directly hashable
 */
#pragma once
#ifndef __TIMEWHALE_INSTANCEID_H_
#define __TIMEWHALE_INSTANCEID_H_
#include <assert.h>

#include <hash_map>

//#include "Timewhale\ApiSpec.h"
//#define ID_TYPE size_t
//#define NON_ID_BLOCK (1<<4)
//#define LOADABLE_ID_BLOCK (1<<16)


//
//
//namespace Timewhale {
//namespace Util {
//	//class _instanceID {
//	//public:
//	//	static _instanceID& GetSingleton() {
//	//		static _instanceID instance;
//	//		return instance;
//	//	}
//	//	static ID_TYPE GetNextID() {
//	//		_instanceID& iid = _instanceID::GetSingleton();
//	//		return iid.nextID++;
//	//	}
//	//	static ID_TYPE GetRemainingIDCount() {
//	//		_instanceID& iid = _instanceID::GetSingleton();
//	//		return iid.idMax - iid.nextID - 1;
//	//	}
//	//	static ID_TYPE GetMinLoadableID() {
//	//		return ID_TYPE(NON_ID_BLOCK + 1);
//	//	}
//	//	static ID_TYPE GetMaxLoadableID() {
//	//		return ID_TYPE(LOADABLE_ID_BLOCK);
//	//	}
//	////operator unsigned int();
//	////operator unsigned int() const;
//
//	////inline bool operator==(InstanceID& other);
//	////inline bool operator==(const InstanceID& other);
//	//
//	//private:
//	//	void take_snapshot() {
//	//		snapshot = nextID;
//	//	}
//	//	void restore_snapshot() {
//	//		if(snapshot != 0) {
//	//			nextID = snapshot;
//	//		}
//	//	}
//	//	_instanceID() 
//	//		: nextID(LOADABLE_ID_BLOCK) , 
//	//		idMax(ID_TYPE(1) << (sizeof(ID_TYPE) * 8 - 1)),
//	//		snapshot(0)
//	//	{};
//	//	_instanceID(_instanceID const&);
//	//	void operator=(_instanceID const&);
//	//	ID_TYPE nextID;
//	//	ID_TYPE idMax;
//	//	ID_TYPE snapshot;
//	//};
//	//static ID_TYPE GetNextID() {
//	//	return _instanceID::GetNextID();
//	//}
//} //Util
//} //Timwwhale
////typedef ID_TYPE InstanceID;

namespace Timewhale {
	struct id_core {
		static const size_t id_error = (size_t)0;
	private:
		static const size_t flag_range = (size_t)((1 << 4) -1);
		static const size_t reserved_range = (size_t)((1 << 16) -1);
		static const size_t id_max = (size_t)((1 << 31));

		size_t cur_reserved_id;
		size_t cur_free_id;

		id_core()
			:cur_reserved_id(flag_range + 1),
			cur_free_id(reserved_range +1)
		{}
	public:
		static id_core& getIDCore() {
			static id_core core;
			return core;
		}

		inline size_t nextReservedID() {
			if(cur_reserved_id == reserved_range) {
				//This is an error, we've run out of reserved ids
				assert(false);
				return id_error;
			}
			return ++cur_reserved_id;
		}

		inline size_t nextFreeID() {
			if(cur_free_id == id_max) {
				//This is an error, we've run out of free ids
				assert(false);
				return id_error;
			}
			return ++cur_free_id;
		}
	};
	
	struct InstanceID {
		size_t id		: 32;
		size_t extra	: 16;
		InstanceID()
			:id(id_core::id_error),
			extra(id_core::id_error)
		{}

		InstanceID(const size_t id) 
			:id(id),
			extra(id_core::id_error)
		{}
		InstanceID(const size_t id, const size_t extra)
			:id(id),
			extra(extra)
		{}
		inline operator size_t() {
			return id;
		}
        inline operator int() const {
            return id;
        }
        inline operator unsigned int() const {
            return id;
        }
		inline const bool operator==(const InstanceID& other) const{
			return id == other.id;
		}
		inline const bool operator==(int other) const {
			return id == (size_t)other;
		}
		inline const bool operator!=(const InstanceID& other) const{
			return id != other.id;
		}
		inline const bool operator<(const InstanceID& other) const{
			return id < other.id;
		}
		inline const bool operator>(const InstanceID& other) const{
			return id > other.id;
		}
		inline operator bool() const{
			return id != id_core::id_error; 
		}
	};

	/*bool operator==(const InstanceID& lhs, const InstanceID& rhs) {
		return lhs.id == rhs.id;
	}
	bool operator!=(const InstanceID& lhs, const InstanceID& rhs) {
		return lhs.id != rhs.id;
	}
	bool operator<(const InstanceID& lhs, const InstanceID& rhs) {
		return lhs.id < rhs.id;
	}
	bool operator>(const InstanceID& lhs, const InstanceID& rhs) {
		return lhs.id > rhs.id;
	}*/
	static const InstanceID error_iid;

	struct SceneID {
		size_t id		: 16;
		SceneID()
			:id(id_core::id_error)
		{}
		SceneID(const size_t id) 
			:id(id)
		{}
		inline operator size_t() {
			return id;
		}
		inline const bool operator==(const SceneID& other) const{
			return id == other.id;
		}
		inline const bool operator!=(const SceneID& other) const{
			return id != other.id;
		}
		inline const bool operator<(const SceneID& other) const{
			return id < other.id;
		}
		inline const bool operator>(const SceneID& other) const{
			return id > other.id;
		}
		inline operator bool() const{
			return id != id_core::id_error;
		}
	};
	/*bool operator==(const SceneID& lhs, const SceneID& rhs) {
		return lhs.id == rhs.id;
	}
	bool operator!=(const SceneID& lhs, const SceneID& rhs) {
		return lhs.id != rhs.id;
	}
	bool operator<(const SceneID& lhs, const SceneID& rhs) {
		return lhs.id < rhs.id;
	}
	bool operator>(const SceneID& lhs, const SceneID& rhs) {
		return lhs.id > rhs.id;
	}*/
	static const SceneID error_sid;

	struct TypeID {
		size_t typeID	: 16;
		TypeID()
			:typeID(id_core::id_error)
		{}
		TypeID(const size_t id) 
			:typeID(id)
		{}
		inline operator size_t() {
			return typeID;
		}
		inline const bool operator==(const TypeID& other) const{
			return typeID == other.typeID;
		}
		inline const bool operator!=(const TypeID& other) const{
			return typeID != other.typeID;
		}
		inline const bool operator<(const TypeID& other) const{
			return typeID < other.typeID;
		}
		inline const bool operator>(const TypeID& other) const{
			return typeID > other.typeID;
		}
		inline operator bool() const{
			return typeID != id_core::id_error;
		}
	};
	/*bool operator==(const TypeID& lhs, const TypeID& rhs) {
		return lhs.typeID == rhs.typeID;
	}
	bool operator!=(const TypeID& lhs, const TypeID& rhs) {
		return lhs.typeID != rhs.typeID;
	}
	bool operator<(const TypeID& lhs, const TypeID& rhs) {
		return lhs.typeID < rhs.typeID;
	}
	bool operator>(const TypeID& lhs, const TypeID& rhs) {
		return lhs.typeID > rhs.typeID;
	}	*/

	static const TypeID error_tid;

	static size_t GetNextID() {
		return id_core::getIDCore().nextFreeID();
	}
	static size_t GetNextReservedID() {
		return id_core::getIDCore().nextReservedID();
	}
}
//hashers for our id types
namespace std {
	template<> struct hash<Timewhale::InstanceID> {
		size_t operator()(const Timewhale::InstanceID& id) {
			return std::hash<size_t>()(id.id);
		}
	};
	template<> struct hash<Timewhale::TypeID> {
		size_t operator()(const Timewhale::TypeID& id) {
			return std::hash<size_t>()(id.typeID);
		}
	};
	template<> struct hash<Timewhale::SceneID> {
		size_t operator()(const Timewhale::SceneID& id) {
			return std::hash<size_t>()(id.id);
		}
	};
}

//typedef unsigned int TypeID;
template<typename T>
struct CTID {
	const Timewhale::TypeID id;
	CTID() : id(Timewhale::GetNextReservedID())
	{};
};

template<typename T>
Timewhale::TypeID ctid() {
	static CTID<T> this_id;
	return this_id.id;
}
/*namespace std {
	template <> struct hash<timewhale::Util::InstanceID> {
		size_t operator()(const timewhale::Util::InstanceID & x) const {
			return hash<unsigned int>()(unsigned int(x));
		}
	};
}*/

#endif