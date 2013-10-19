#pragma once
#ifndef __TIMEWHALE_EVENT_H_
#define __TIMEWHALE_EVENT_H_
#include <type_traits>

#include "InstanceID.h"

#include <ppltasks.h>
#include <ppl.h>
#include <vector>
#include <functional>

namespace Timewhale {
	
	//Do nothing struct that is inheritable to allow passing args to an event.
	struct EventArgs {
		EventArgs() {}
		virtual ~EventArgs() {};
	};

	template<class T>
	struct EventHandler {
		size_t mID;
		std::function<void (const T&)> handler;
		EventHandler(const std::function<void (const T&)>& handler_func)
			:handler(handler_func),
			mID(GetNextReservedID())
		{}
		EventHandler() 
			:handler(nullptr),
			mID(id_core::id_error)
		{}
		bool operator==(const EventHandler<T>& other) {
			return mID == other.mID;
		}
		bool operator!=(const EventHandler<T>& other) {
			return mID != other.mID;
		}
	};

	template<class T>
	class Event {
		typedef void(*EventFuncPtr)(const T&) ;
		static_assert(std::is_base_of<EventArgs, T>::value, "Event must be templated with a valid EventArgs struct");
		struct event_internal {
			friend class Event<T>;
			typedef std::vector<EventHandler<T> >  handler_vec;

			inline event_internal(const InstanceID& id) 
				: eventID(id)
			{}

			inline event_internal(event_internal&& other) 
				:eventID(other.eventID),
				mHandlers(std::move(other.mHandlers)) 
			{}
		protected:
			handler_vec mHandlers;
			InstanceID eventID;

			inline EventHandler<T>& addHandler(const EventHandler<T>& handler_func) {
				//log_sxinfo("EVENT", "Target: %p", handler_func.target<EventFuncPtr>());

				mHandlers.push_back(handler_func);
				return *(mHandlers.end() -1);
			}

			inline bool removeHandler(const EventHandler<T>& handler_func) {
				/*for(auto it = mHandlers.begin(); it != mHandlers.end(); ++it) {
					if((*it).target<EventFuncPtr>() == handler_func.target<EventFuncPtr>()) {
						mHandlers.erase(it);
						return true;
					}
				}
				return false;*/
				auto found = std::find(mHandlers.begin(), mHandlers.end(), handler_func);
				if(found != mHandlers.end()) {
					mHandlers.erase(found);
					return true;
				}
				return false;
			}
		};

	private:
		typedef std::function<void (const T&)> EventFunc;
		typedef std::unique_ptr<event_internal> internals;
		internals mInternal;

	public:
		inline Event<T>(Event<T>&& other) 
			: mInternal(std::move(other.mInternal)) 
        {}

		inline Event<T>() 
			:mInternal(new event_internal(GetNextReservedID()))
		{}
		inline ~Event() {
			if(mInternal) mInternal.release();
		}

		inline void Call(const T& args) {
			if(!mInternal) return;//throw new runtime_exception("Error! Attempting to call moved Event!");
			for(auto& func : mInternal->mHandlers) {
				func.handler(args);
			}
		}

		inline void operator()(const T& args) {
			Call(args);
		}

		inline EventHandler<T>& operator += (const EventFunc& f) {
			return mInternal->addHandler(f);

		}

		inline EventHandler<T>& AddHandler(const EventFunc& f) {
			return (*this += f);
		}

		inline bool operator -= (const EventHandler<T>& f) {
			return mInternal->removeHandler(f);
		
		}

		inline bool RemoveHandler(const EventHandler<T>& f) {
			return (*this -= f);
		}
	};



}

#endif
