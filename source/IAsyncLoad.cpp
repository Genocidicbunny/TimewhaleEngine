// Async Load Implementation file

#include "IAsyncLoad.h"
#include "ResourceManager.h"

namespace Timewhale
{
	//IAsyncLoad::IAsyncLoad(const std::string& name, ResourceType type, void(*func)(const std::string&, uint32_t tag))
	//{
	//	_task = std::thread(RunTask, name, type, func);
	//}

	//IAsyncLoad::~IAsyncLoad()
	//{
	//	if (_task.joinable())
	//		_task.join();
	//}

	//int IAsyncLoad::RunTask(const std::string& name, ResourceType type, void(*func)(const std::string&, uint32_t tag))
	//{
	//	ResourceManagerPtr rm = ResourceManager::get();

	//	twResource* rsc = rm->Load(name, type);

	//	func(name, rsc->getTag());

	//	return 0;
	//}
}