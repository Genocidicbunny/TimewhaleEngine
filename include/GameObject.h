/*
 *Base GameObject class.
 *All Entities and Components derive from this class. Various subsystems instead derive from EngineObject which is not exposed
 */

#ifndef __TIMEWHALE_GAMEOBJECT_H_
#define __TIMEWHALE_GAMEOBJECT_H_

//#include <wchar.h>
#include <string>
#include <mutex>
#include <unordered_map>
#include <memory>
#include "cppBtree\btree_map.h"
#include "InstanceID.h"
#include "TWLogger.h"

#define ECS_info(format, ...) log_sxinfo("ECS", format, ##__VA_ARGS__)
#define ECS_warn(format, ...) log_sxwarn("ECS", format, ##__VA_ARGS__)
#define ECS_debug(format, ...) log_sxdebug("ECS", format, ##__VA_ARGS__)
#define ECS_error(format, ...) log_sxerror("ECS", format, ##__VA_ARGS__)

namespace Timewhale {

	namespace Tag {
		static std::string MissingTag("MissingTag");
	}

	typedef std::shared_ptr<std::string> GameObjTag;
	typedef btree::btree_map<std::string, GameObjTag> TagPtrMap;
	typedef TagPtrMap::iterator TPMitor;
	typedef TagPtrMap::const_iterator TPMcitor;
	typedef TagPtrMap::reverse_iterator TPMritor;

	using namespace Timewhale;
	class GameObject {
		friend class ComponentManager;
		friend class EntityManager;
		friend class Component;
		friend class Entity;
		//protected:
	public:
			static std::unique_ptr<TagPtrMap> TagCache;
			static std::recursive_mutex tagCacheMtx;
			/* This constructor will automatically assign a new InstanceID to the newly created object
			 * Although no tag is provided, the created game object will not have a nullptr for the tag.
			 * This is because the tag is used for hashing within the Component and Entity managers
			 * and thus would involve unnecessary branching logic to deal with the special case of nullptr
			 * Thus, this constructor assigns the newly created GameObject the "MissingTag" object tag
			 * Because of this, any GameObject manager will not need special logic for missing tags
			 */
			GameObject();

			GameObject(const std::string& tag);

			/* This constructor will create a copy of the passed-in GameObject. The newly created
			 * GameObject will though be assigned a new id as no two GameObjects can share an id.
			 * The GameObject tag will remain the same, although it will be allocated it's own storage.
			 *\param	GameObject&		other			The GameObject that is to be cloned
			 */
			GameObject(const GameObject& other);

			/* Override of the default assignment operator to ensure that the newly copied GameObject bears
			 * its own unique ID and has properly allocated storage for it's tag
			 *\param	GameObject&		other			The game object to be copied
			 *
			 *\return	GameObject&		A reference to a copy of the passed-in GameObject
			 */
			GameObject &operator= (const GameObject& other);

			/*! 
			 * Operator== override. Returns true if the two GameObjects have the same id
			 */
			const bool operator==(const GameObject& other) const;

			/*!
			 * Operator== override. Returns true if the two GameObjects have the same id
			 */
			bool operator==(GameObject& other) ;

			/*! 
			 * Operator!= override. Returns true if the two GameObjects have the same id
			 */
			const bool operator!=(const GameObject& other) const;

			/*!
			 * Operator!= override. Returns true if the two GameObjects have the same id
			 */
			bool operator!=(GameObject& other) ;


			/* This destroys the GameObject, freeing the space allocated for it's tag. 
			 * Thus any pointer's to the GameObject's tag will become invalid on destruction
			 */
			virtual ~GameObject();
		public:

			/* Returns the ID of this object
			 *\return	InstanceID		The ID of this object
			 */
			const InstanceID getID() const;

			/* Returns a reference to this GameObject's tag
			 * appropriate functions
			 *\return	string			A ref to either a copy or the actual tag of a GameObject
			 */
			std::string& getTag();

			/*! Returns a const pointer to this GameObject's tag
			 * Since the returned pointer is const, we can not worry about making a copy
			 *\return	string&			A const ref to a copy of this GameObject's tag
			 */
			const std::string& getCTag() const;

			/*! Returns a const pointer to this Object's tag
			* \return char* A const pointer to this object's tag
			*/
			const char* getCStrTag() const;
			/*!
			 * This function returns true if this GameObject's tag is equal
			 * to the passed in one, false otherwise
			 *\param	otherTag		A pointer to a GameObject tag
			 *
			 *\return	bool			A boolean value, true if this GameObject's tag is equal to
			 *							otherTag, false otherwise
			 */
			const bool compareTag(const std::string& otherTag) const;

			/*!
			 * This function returns true if this GameObject's id is equal to the passed
			 * in one. False otherwise
			 *\param	otherID		The GameObject id to compare to
			 *
			 *\return	bool		A boolean value, true if this GameObject's id is equal
			 *					to otherID
			 */
			const bool compareID(const InstanceID otherID) const;
			
			/* This function sets the tag of a GameObject to the tag passed in.
			 * This function is marked as virtual because various inheriting classes
			 * may require alternate behaviour for setting the tag of an object.
			 * By default this function simply creates a copy of the passed-in string
			 * and copies it to it's own tag string.
			 *\param	wchar_t*		New tag to assign to this GameObject
			 */
			void setTag(const std::string&);

			static const std::string getLogTag();


		protected:
			GameObjTag GetCachedTag(const std::string& tag);

			//std::string mTag;
			GameObjTag mTag;
			InstanceID mID;


	};
}

#endif
