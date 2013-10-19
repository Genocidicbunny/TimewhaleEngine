#pragma once
#ifndef __TIMEWHALE_ICOMPONENT_H_
#define __TIMEWHALE_ICOMPONENT_H_

#include <memory>
#include <bitset>
#include <mutex>
#include <typeindex>
#include <cassert>
#include "GameObject.h"
#define ICOMPONENT_FLAGS_SIZE 8
namespace Timewhale {
	class ECSManager;
	class IComponent : public GameObject {
		friend class ECSManager;
	private:
		bool mActive;
		unsigned long hardFlags;
		InstanceID parentID;
		TypeID mTypeID;
	public:
		IComponent() 
			: GameObject(),
			mActive(true),
			parentID(),
			mTypeID()
		{
		};
		virtual ~IComponent() {}

		
		virtual void PreUpdate() {};
		virtual void Update() {};
		virtual void PostUpdate() {};
		//std::shared_ptr<const IComponent> GetPtr() const {
		//	return shared_from_this();
		//}
		//activation
		/*!
		 * This function will return the current activity status of a component
		 *\return		Boolean flag, true for active, false for inactive
		 */
		const bool IsActive() const {
			return mActive;
		}
		/*!
		 * This function will activate the component if it is not already active
		 * In addition, it will call the component's OnActive(), but only if the 
		 * component was previously inactive. Thus there is no penalty for calling this
		 * on an already active component.
		 */
		void Activate() {
			ECS_info("Activating component. ID: %10d, Parent.ID: %10d", getID(), parentID);
			if(!mActive) {
				mActive = true;
				OnActivate();
			}
		}
		/*!
		 * This function will deactivate the component if it is not already  deactivated
		 * In adition it will call the component's OnDeactivate(), but only if the
		 * component was previously active. Thus there is no penalty for calling this
		 * on an already active component.
		 */
		void Deactivate() {
			ECS_info("Deactivating component. ID: %10d, Parent.ID: %10d", getID(), parentID);
			if(mActive) {
				mActive = false;
				OnDeactivate();
			}
		}
		/*!
		 * This function will set the components active state to the provided value.
		 * This function is simply a wrapper around activate() and deactivate(), 
		 * calling the appropriate one depending on the passed in value.
		 * Thus it will also appropriately call the component's OnActivate() and OnDeactivate();
		 *\param	activeState			Flag to use when setting the components active state
		 */
		void SetActive(bool activeState) {
			activeState ? Activate() : Deactivate();
		}

		//parent Entity access
		/*!
			* This function returns a pointer to this Component's
			* current parent.
			*\return		A pointer to this component's parent Entity
			*/
		const InstanceID GetParentID() const {
			return parentID;
		}
		/*!
			* This function sets this component's parent Entity to
			* the specified pointer.
			* Note that this does not attach this component to that entity. If you wish to
			* attach this component to an entity, use Entity.AttachComponent(this)
			*\param		parent			A pointer to the Entity to use as this component's parent
			*/
		void SetParent(const InstanceID parent) {
			parentID = parent;
		}
		/*const uint32_t getPriority() const {
			return m_Priority;
		}
		void setPriority(uint32_t priority) {
			iComponentMtx.lock();
			m_Priority = priority;
			iComponentMtx.unlock();
		}*/
		const bool IsFlagSet(unsigned long otherFlags) const {
			return (hardFlags & otherFlags) != 0;
		}
		const unsigned long GetFlags() const {
			return this->hardFlags;
		}
		const std::bitset<ICOMPONENT_FLAGS_SIZE> GetFlagsSet() const {
			return std::bitset<ICOMPONENT_FLAGS_SIZE>(hardFlags);
		}
	//Events
	public:
		/*!
		 * This event handler is called when the Component is activated. This is useful
		 * if the entity needs to free up some resources when its deactivated
		 * and reaquire them on activation. This handler is also called on Component creation
		 * unless the component's active state is set to false on creation
		 */
		virtual void OnActivate() 
		{
			ECS_debug("OnActivate called on IComponent. ID: %10d", getID());
		};
		/*!
		 * This event handler is called when the Component is deactivated
		 */
		virtual void OnDeactivate() 
		{
			ECS_debug("OnDeactivate called on IComponent. ID: %10d", getID());
		};

		virtual void OnDestroy()
		{
			ECS_debug("OnDestroy called on IComponent. ID: %10d", getID());
		}

				
		/*!
		 * This function is called after a component has been created. The created
		 * component is guaranteed to have an assigned ID and an assigned tag. It is also
		 * guaranteed to have been attached to a parent entity and inserted into the component
		 * manager.
		 */
		virtual void Init() {
			ECS_debug("Init called on IComponent. ID: %10d", getID());
		};

	protected:
		void SetFlags(const std::bitset<8>& newFlags) {
			this->hardFlags = newFlags.to_ulong();
			ECS_debug("Setting flags. ID: %10d, Flags: %x", getID(), hardFlags);
		}
		void SetFlags(const unsigned long newFlags) {
			this->hardFlags = newFlags;
			ECS_debug("Setting flags. ID %10d, Flags: %x", getID(), hardFlags);
		}
	};
}

#endif
