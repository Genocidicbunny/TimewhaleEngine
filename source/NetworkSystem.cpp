#include "NetworkSystem.h"
#include "CRC32.h"
#include "TWLogger.h"
#include <Windows.h>

namespace Timewhale {

	NetworkSystemPtr NetworkSystem::sSystem = nullptr;

	NetworkSystem::NetworkSystem() :
		checksum(0)
	{
		memset(checksumStr, 0, 12);
		log_sxinfo("NetworkSystem", "NetworkSystem created!");
	}

	NetworkSystem::~NetworkSystem() {
		log_sxinfo("NetworkSystem", "NetworkSystem shutting down");
		//Shutdown will work even if we've already Shutdown before
		Shutdown();
	}

	bool NetworkSystem::Initialize() {
		log_sxinfo("NetworkSystem", "Begin Initialize");
		static wchar_t exeName[512];
		if(GetModuleFileName(0, exeName, 512) == 0) { 
			log_sxerror("NetworkSystem", "Failed to get executable name");	
			return false;
		}
		log_sxinfo("NetworkSystem", "Self Module filename: %ls", exeName);
		log_sxinfo("NetworkSystem", "Begin CRC generation");
		checksum = CRC32::FileCrc32(exeName);
		log_sxinfo("NetworkSystem", "End CRC generation");
		if(checksum == 0){
			log_sxerror("NetworkSystem", "Failed to generate checksum for executable. Cannot continue");
			return false;
		}
#pragma warning(disable: 4996)
		sprintf(checksumStr, "%u", checksum);
		log_sxinfo("NetworkSystem", "Self CRC: %s", checksumStr);
		log_sxinfo("NetworkSystem", "End Initialize");
		return true;
	}
	void NetworkSystem::Update() {
		for(auto iface : mInterfaces) {
			if(iface) iface->Update();
		}
	}

	void NetworkSystem::Shutdown() {
		for(auto iface : mInterfaces) {
			iface->Shutdown();
			delete iface;
		}
		mInterfaces.clear();
	}

	NetworkSystemPtr const& NetworkSystem::create() {
		if(!sSystem) {
			sSystem = NetworkSystemPtr(new NetworkSystem());
		}
		return sSystem;
	}
}