/*
 *Base GameObject implementation
 */


#include "GameObject.h"

namespace Timewhale {

	std::unique_ptr<TagPtrMap> GameObject::TagCache(new TagPtrMap());// = std::make_shared<TagPtrMap>();
	std::recursive_mutex GameObject::tagCacheMtx;

	/*! 
	 * This constructor will automatically assign a new InstanceID to the newly created object
	 * Although no tag is provided, the created game object will not have a nullptr for the tag.
	 * This is because the tag is used for hashing within the Component and Entity managers
	 * and thus would involve unnecessary branching logic to deal with the special case of nullptr
	 * Thus, this constructor assigns the newly created GameObject the "MissingTag" object tag
	 * Because of this, any GameObject manager will not need special logic for missing tags
	 */
	GameObject::GameObject() 
		: mID(GetNextID())
	{
		ECS_warn("Creating GameObject with no tag specified! ID: %10d", mID);
		mTag = GetCachedTag(Tag::MissingTag);
	}
	GameObject::GameObject(const std::string& tag) 
		:mID(GetNextID())
	{
		if(tag.length() > 0) {
			mTag = GetCachedTag(tag);
		} else {
			mTag = GetCachedTag(Tag::MissingTag);
		}
		ECS_info("Creating GameObject ID: %10d, Tag: %s", mID.id, mTag->c_str());
	}


	/*! This constructor will create a copy of the passed-in GameObject. The newly created
	 * GameObject will though be assigned a new id as no two GameObjects can share an id.
	 * The GameObject tag will remain the same, although it will be allocated it's own storage. 
	 *\param	other			The GameObject that is to be cloned
	 */
	GameObject::GameObject(const GameObject& that) 
		: mID(GetNextID()),
		mTag(that.mTag)
	{
		ECS_info("Cloning GameObject. Other.ID: %10d, ID: %10d, Other.Tag: %s, Tag: %s", that.mID, mID, that.mTag->c_str(), mTag->c_str());
	}

	/*! Override of the default assignment operator to ensure that the newly copied GameObject bears
	 * its own unique ID and has properly allocated storage for it's tag
	 *\param	other			The game object to be copied
	 *
	 *\return	GameObject&		A reference to a copy of the passed-in GameObject
	 */
	GameObject& GameObject::operator= (const GameObject& that) {
		ECS_info("Cloning GameObject. Other.ID: %10d, ID: %10d, Other.Tag: %s, Tag: %s", that.mID, mID, that.mTag->c_str(), mTag->c_str());
		if (this == &that) return *this;
		mID = GetNextID();
		return *this;
	}

	/*! This destroys the GameObject, freeing the space allocated for it's tag. 
	 * Thus any pointer's to the GameObject's tag will become invalid on destruction
	 */
	GameObject::~GameObject() 
	{
		ECS_info("Destroying GameObject. ID: %10d, Tag: %s", mID.id, mTag->c_str());
	}

	/*! Returns the ID of this object
	 *\return	uint	The ID of this object as an unsigned integer
	 */
	const InstanceID GameObject::getID() const{
		return mID;
	}

	const bool GameObject::compareID(const InstanceID otherID) const {
		return (mID == otherID);
	}

	const bool GameObject::operator!=(const GameObject& other) const {
		return (this->mID != other.mID);
	}
	bool GameObject::operator!=(GameObject& other) {
		return (this->mID != other.mID);
	}

	const bool GameObject::operator==(const GameObject& other) const {
		return (this->mID == other.mID);
	}
	bool GameObject::operator==(GameObject& other) {
		return (this->mID == other.mID);
	}
	GameObjTag GameObject::GetCachedTag(const std::string& tag) {
		GameObjTag tagPtr;
		TPMitor tagFinder = GameObject::TagCache->find(tag);
		if(tagFinder == GameObject::TagCache->end()) {
			ECS_debug("Could not find cached tag for %s. Generating a new cache entry", tag.c_str());
			std::pair<TPMitor, bool> newtag = 
				GameObject::TagCache->insert(make_pair(tag, std::make_shared<std::string>(tag)));
			if(newtag.second) {
				tagPtr = newtag.first->second;
			}
		} else {
			ECS_debug("Cached tag found for %s", tag.c_str());
			tagPtr = tagFinder->second;
		}
		return tagPtr;
	}

	void GameObject::setTag(const std::string& tag) {
		if(tag.length() == 0) {
			ECS_error("Attempting to set GameObject tag to empty tag. Tag will be set to MissingTag");
			mTag = GetCachedTag("MissingTag");
		}
	}

	const std::string& GameObject::getCTag() const {
		return *mTag;
	}
	const char* GameObject::getCStrTag() const {
		return mTag->c_str();
	}
	const bool GameObject::compareTag(const std::string& tag) const {
		return (*mTag == tag);
	}
	//Logging:
	const std::string GameObject::getLogTag(){
		return "GameObject";
	}


}
