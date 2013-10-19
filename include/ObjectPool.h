#pragma once
#ifndef __TIMEWHALE_OBJECTPOOL_H_
#define __TIMEWHALE_OBJECTPOOL_H_
#include <vector>

namespace Timewhale {

	//template<class T>
	//class ObjectPool {
	//private:
	//	static std::vector<T> pool;
	//	static std::vector<T*> pool_ptr;
	//	size_t currentSize;
	//	static ObjectPool<T>* instance;
	//	ObjectPool() 
	//		:currentSize(0)
	//	{
	//	}
	//public:
	//	template<class T>
	//	static ObjectPool<T>* GetInstance() {
	//		if(!instance) instance = new ObjectPool();
	//		return instance;
	//	}
	//	inline bool ReservePoolSize(const size_t newSize) {
	//		size_t diff = newSize = currentSize;
	//		if(diff <= 0) return true;

	//		pool.reserve(newSize);


	//	}
	//};


}


#endif