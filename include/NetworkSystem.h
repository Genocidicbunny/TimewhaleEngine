#pragma once
#ifndef __TIMEWHALE_NETWORKSYSTEM_H_
#define __TIMEWHALE_NETWORKSYSTEM_H_
#include "InstanceID.h"
#include "TWLogger.h"
#include "StringUtils.h"
#include <memory>
#include <vector>
#include <algorithm>
#include <type_traits>

namespace Timewhale {

	class INetworkInterface {
		friend class NetworkSystem;
	private:
		InstanceID interfaceIID;

		//No copy or assignment allowed
		INetworkInterface(const INetworkInterface&);
		INetworkInterface& operator=(const INetworkInterface&);
	public:
		//Creation interface
		//INetworkInterface() will get automatically called so we will always get an interfaceIID assigned to us
		//This is also protected so the interface cannot be directly created
		INetworkInterface()
			:interfaceIID(GetNextReservedID())
		{
		}
		virtual ~INetworkInterface() {}

		//NetworkSystem interface

		//Should initialize the implementation to such a state where it can be used and have Update() called on it
		virtual bool Initialize() = 0;
		//Called once per engine tick (nominal 16ms) and can be used to performed continuous operations
		virtual void Update() = 0;
		//Should bring the interface to a state where it can safely be initialized again. Any resources allocated in Initialize()
		//Should therefore be freed here, whereas resources allocated in the constructor should not be (unless they need to be, as per implementation)
		virtual void Shutdown() = 0;

		//ID Getter (cannot be manually set)
		//the const& prevents anyone changing the IID and makes just getting the
		//'id' part of the interfaceIID much simpler
		inline const uint32_t GetID() const {
			return interfaceIID.id;
		}

		//Comparison operators for NetworkSystem to use
		const bool operator==(INetworkInterface const& other) const {
			return interfaceIID == other.interfaceIID;
		}
		const bool operator!=(INetworkInterface const& other) const {
			return !operator==(other);
		}
		const bool operator<(INetworkInterface const& other) const {
			return interfaceIID < other.interfaceIID;
		}
		const bool operator>(INetworkInterface const& other) const {
			return interfaceIID > other.interfaceIID;
		}
	};
	typedef std::vector<INetworkInterface*> NetworkInterfaceVec;

	class NetworkSystem {
	private:
		uint32_t checksum;
		NetworkInterfaceVec mInterfaces;
		char checksumStr[12];

		
		static std::shared_ptr<NetworkSystem> sSystem;

		NetworkSystem();
	public:
		~NetworkSystem();
		//creator
		static std::shared_ptr<NetworkSystem> const& create();

		//getter
		static std::shared_ptr<NetworkSystem> const& get() {
			return sSystem;
		}
		static uint32_t getChecksum() {
			auto& sSys = get();
			return sSys ? sSys->checksum : 0;
		}
		static char const* getChecksumStr() {
			auto& sSys = get();
			return sSys ? sSys->checksumStr : "\0";
		}

		//Interface to rest of engines
		bool Initialize();
		void Update();
		void Shutdown();

		template<class NInterface>
		NInterface* const GetInterface() {

			static_assert(std::is_base_of<INetworkInterface, NInterface>::value, 
				"Specified class is not a valid network interface! " __FILE__ ":" STR(__LINE__));
			static_assert(!std::is_same<INetworkInterface, NInterface>::value, 
				"Cannot create an abstract NetworkInterface! " __FILE__ ":" STR(__LINE__));

			NInterface* newIface = new NInterface;
			newIface->Initialize();
			mInterfaces.push_back(newIface);
			return newIface;
		}

		//takes a pointer to a pointer to a NInterface and will set it to nullptr when it's done
		template<class NInterface>
		void ReleaseInterface(NInterface** iface) {

			static_assert(std::is_base_of<INetworkInterface, NInterface>::value, 
				"Specified class is not a valid network interface! " __FILE__ ":" STR(__LINE__));
			static_assert(!std::is_same<INetworkInterface, NInterface>::value, 
				"Cannot release an abstract NetworkInterface! " __FILE__ ":" STR(__LINE__));

			if(!iface || !*iface) return;

			
			auto finder = std::find_if(mInterfaces.begin(), mInterfaces.end(), 
				[iface](INetworkInterface const* i) -> bool 
				{ 
					if(!i) return false;
					return (i->interfaceIID == (*iface)->interfaceIID);
				});
			if(finder == mInterfaces.end()) {
				log_sxerror("NetworkSystem", "Interface to be released is not managed");
				return;
			}
			mInterfaces.erase(finder);
			(*iface)->Shutdown();
			delete *iface;
			*iface = nullptr;
		}
	private:

	};

	typedef std::shared_ptr<NetworkSystem> NetworkSystemPtr;
	

}

#endif