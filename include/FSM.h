#pragma once
#ifndef __TIMEWHALE_FSM_H_
#define __TIMEWHALE_FSM_H_
#include <memory>
#include <functional>
#include <cstdint>
#include <unordered_map>


#include "TWLogger.h"

namespace Timewhale {
	
	typedef uint32_t FS_ID;
	static const FS_ID FS_ID_Error = 0;

	template<typename EventType>
	class FSM {
	public: 
		//the handler should return false of no change is needed, true if it is. If a change is needed, the second parameter should be set to the appropriate value
		typedef std::function<bool (EventType, std::string&)> FSM_EventHandler;
	private:
		typedef std::unordered_map<std::string, FSM_EventHandler> FSM_States;

		FSM_States mStates;
		std::string current_state;
		FSM_EventHandler current_handler;

		std::string next_state;

		const std::string FSM_name;
	public:

		FSM(const std::string& name)
			:FSM_name(name)
		{}
		virtual ~FSM() {}

		/*bool CreateState(std::string& name, FSM_EventHandler const& cbEvent = nullptr);

		bool SetHandler(std::string const& name, FSM_EventHandler const& cbEvent = nullptr);

		bool DeleteState(std::string const& name);

		void Change(std::string const& name);

		bool Update(EventType evt);

		void DumpToLog();*/

		bool CreateState(std::string& name, FSM_EventHandler const& cbEvent = nullptr) {
			auto inserter = mStates.emplace(std::make_pair(name, cbEvent));

			if(!inserter.second) {
				log_sxinfo(FSM_name.c_str(), "Create Error, state \"%s\" already exists", name.c_str());
				return false;
			} return true;
		}

		bool SetHandler(std::string const& name, FSM_EventHandler const& cbEvent = nullptr) {
			auto finder = mStates.find(name);

			if(finder != mStates.end()) {
				finder.second = cbEvent;
				return true;
			} else {
				log_sxerror(FSM_name.c_str(), "SetHandler Error, state \"%s\" does not exist", name.c_str());
				return false;
			}
		}

		bool DeleteState(std::string const& name) {
			auto finder = mStates.find(name);

			if(finder != mStates.end()) {
				mStates.erase(finder);
				return true;
			} else {
				log_sxerror(FSM_name.c_str(), "Delete Error, state \"%s\" does not exist", name.c_str());
				return false;
			}
		}

		bool Change(std::string const& name) {
#ifdef _DEBUG
			auto finder = mStates.find(name);
			if(finder == mStates.end()) {
				log_sxerror(FSM_name.c_str(), "Change Error, state \"%s\" does not exist", name.c_str());
				return false;
			}
#endif
			next_state = name;
			return true;
		}

		bool Update(EventType evt) {

			if(next_state.length() != 0) {
				auto finder = mStates.find(next_state);
				if(finder != mStates.end()) {
					current_state = next_state;
					current_handler = finder.second;
					next_state.clear();
				} else {
					log_sxerror(FSM_name.c_str(), "Update Error, next_state \"%s\" does not exist", next_state.c_str());
					next_state.clear();
					return false;
				}
			}
			//Continue with message update
			static next_str;
			next_str.clear();
			bool need_change = current_handler(evt, next_str);
			if(need_change) return Change(next_str);
			return true;
		}

		void DumpToLog() {
			log_sxinfo(FSM_name.c_str(), "\tFSM Log Dump [%s]", FSM_name.c_str());
			for(auto& state : mStates) {
				log_sxinfo(FSM_name.c_str(), "\t\tFS: \"%s\" Handler: %p", state.first.c_str(), state.second.target());
			}
		}

	};
}

#endif