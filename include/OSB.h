#pragma once
/*!
 *Object State and Object State buffer implementation
 */

#ifndef __TIMEWHALE_OBJECTSTATE_H_
#define __TIMEWHALE_OBJECTSTATE_H_

#include <cassert>
#include <memory>
#include <unordered_map>

#include "InstanceID.h"
#include "ECS.h"



namespace Timewhale {

	enum BufType {
		NoBuf = 0,
		DynamicBuf = 1,
		StaticBuf = 2
	};


	struct OSB {
		InstanceID		mEntityID;
		EntityPtr		entityPtr;
		IComponentPtr	transformPtr;
		std::vector<IComponentPtr>	spritePtrs;

		OSB(const InstanceID& id, const EntityPtr& ptr)
			:mEntityID(id),
			entityPtr(ptr),
			transformPtr(nullptr)
		{}
	};
	typedef std::shared_ptr<OSB> OSBPtr;

	struct RenderBuffer {
		float x, y;
		float width, height;
		float rotationRadians;
        float layer;
		size_t textureID;
		size_t currentSpriteFrame;
        size_t numHorizontalSpriteFrames;
		size_t numVerticalSpriteFrames;
		float r, g, b;

		RenderBuffer()
			:x(0.0f), y(0.0f),
			width(0.0f), height(0.0f),
			rotationRadians(0.0f),
            layer(0.0f),
			textureID(0),
			currentSpriteFrame(0),
            numHorizontalSpriteFrames(0),
			numVerticalSpriteFrames(0),
			r(0.0f), g(0.0f), b(0.0f)
		{}
		RenderBuffer(const float _x, const float _y, const float _w, const float _h, const float _rr, const float _layer, 
			const size_t tex, const size_t frm, const size_t nhfrms, const size_t nvfrms, const float _r, const float _g, const float _b) _NOEXCEPT
			:x(_x), y(_y),
			width(_w), height(_h),
			rotationRadians(_rr),
            layer(_layer),
			textureID(tex),
			currentSpriteFrame(frm),
            numHorizontalSpriteFrames(nhfrms),
			numVerticalSpriteFrames(nvfrms),
			r(_r), g(_g), b(_b)
		{}
		RenderBuffer(const RenderBuffer& other) _NOEXCEPT
			:x(other.x),
			y(other.y),
			width(other.width),
			height(other.height),
			rotationRadians(other.rotationRadians),
            layer(other.layer),
			textureID(other.textureID),
			currentSpriteFrame(other.currentSpriteFrame),
			numHorizontalSpriteFrames(other.numHorizontalSpriteFrames),
			numVerticalSpriteFrames(other.numVerticalSpriteFrames),
			r(other.r), g(other.g), b(other.b)
		{}
		RenderBuffer( RenderBuffer&& other) _NOEXCEPT
			:x(other.x),
			y(other.y),
			width(other.width),
			height(other.height),
			rotationRadians(other.rotationRadians),
            layer(other.layer),
			textureID(other.textureID),
			currentSpriteFrame(other.currentSpriteFrame),
            numHorizontalSpriteFrames(other.numHorizontalSpriteFrames),
			numVerticalSpriteFrames(other.numVerticalSpriteFrames),
			r(other.r), g(other.g), b(other.b)
		{}

		const bool operator==(const RenderBuffer& other)  _NOEXCEPT {
			return 
			(
				x == other.x &&
				y == other.y &&
				width == other.width &&
				height == other.height &&
				rotationRadians == other.rotationRadians &&
                layer == other.layer &&
				textureID == other.textureID &&
				currentSpriteFrame == other.currentSpriteFrame &&
                numHorizontalSpriteFrames == other.numHorizontalSpriteFrames &&
				numVerticalSpriteFrames == other.numVerticalSpriteFrames &&
				r == other.r &&
				g == other.g &&
				b == other.b
			);
		}
		const bool operator!=(const RenderBuffer& other) _NOEXCEPT{
			return !this->operator==(other);
		}
		RenderBuffer& operator=(const RenderBuffer& other)  _NOEXCEPT{
			if(this != &other) {
				x = other.x;
				y = other.y;
				width = other.width;
				height = other.height;
				rotationRadians = other.rotationRadians;
                layer = other.layer;
				textureID = other.textureID;
				currentSpriteFrame = other.currentSpriteFrame;
                numHorizontalSpriteFrames = other.numHorizontalSpriteFrames;
				numVerticalSpriteFrames = other.numVerticalSpriteFrames;
				r = other.r;
				g = other.g;
				b = other.b;
			}
			return *this;
		}
		RenderBuffer& operator=(RenderBuffer&& other)  _NOEXCEPT {
			if(this != &other) {
				x = other.x;
				y = other.y;
				width = other.width;
				height = other.height;
				rotationRadians = other.rotationRadians;
                layer = other.layer;
				textureID = other.textureID;
				currentSpriteFrame = other.currentSpriteFrame;
                numHorizontalSpriteFrames = other.numHorizontalSpriteFrames;
				numVerticalSpriteFrames = other.numVerticalSpriteFrames;
				r = other.r;
				g = other.g;
				b = other.b;
			}
			return *this;
		}


	};

    struct rBufLess {
        bool operator() (const RenderBuffer &lhs, const RenderBuffer &rhs) const {
            return (lhs.layer < rhs.layer);
        }
    };

	struct LayerInfo {
		size_t Layer;
		size_t NumElements;
		std::vector<RenderBuffer> rbuf_vec;
		bool FlushFlag; // false means no changed data for this layer

		/*~LayerInfo() {
			for(auto rb : rbuf_vec) {
				if(rb) delete rb;
			}
		}*/
	};
	class Camera;
	struct SceneRenderData {
		Camera* SceneCamera;
		//size_t SceneRelativeLayer;
		//LayerInfo layers[(1 <<4)];
        std::vector<RenderBuffer> rbuf_vec;


		SceneRenderData() 
		//	:SceneRelativeLayer(0)
		{
		//	size_t layerCount = 0;
		//	for(auto& l : layers) {
		//		l.Layer = layerCount++;
		//	}
		}
		SceneRenderData(const size_t rel)
		//	:SceneRelativeLayer(0)
		{
		//	size_t layerCount = 0;
		//	for(auto& l : layers) {
		//		l.Layer = layerCount++;
		//	}
		}
	};

//
//	using std::shared_ptr;
//	
//	//ALIGN_AS(16) class ObjectStateBuf : public AlignedNew<ObjectStateBuf> {
//	//	unsigned int bufID;
//
//	//	//Need to provide a parameter here for each buffer type. Should default to some value
//	//	ObjectStateBuf(const unsigned int id, const BufType RenderBufType = NoBuf, const BufType TransformBufType = NoBuf, ObjectStateBuf* parent = nullptr);
//	//	~ObjectStateBuf();
//	//private:
//	//	//Add pimpl classes here as more buffer types are implemented
//	//	//class RenderBufImpl;
//	//	class NodeBufImpl;
//	//	class TransformBuf2DImpl;
//	//	class RenderBufImpl;
//	//	
//	//	//Pointers to pimpls
//	//	NodeBufImpl* pNode;
//	//	RenderBufImpl* pRImpl;
//	//	TransformBuf2DImpl* pTImpl;
//
//
//	//	ObjectStateBuf(const ObjectStateBuf&);
//	//	ObjectStateBuf& operator=(const ObjectStateBuf&);
//
//	//	void AddNode();
//	//	
//	//public:
//	//	void AddRenderBuffer(const BufType type, const std::function<void (RenderBufImpl*)>& initFunc);
//	//	void AddRenderBuffer(const BufType type);
//	//	void AddTransformBuffer(const BufType, const std::function<void (TransformBuf2DImpl*)>& initFunc);
//	//	void AddTransformBuffer(const BufType type);
//	//	const bool isNode() const;
//
//	//public:
//	//	const ObjectStateBuf* getParent()const ;
//	//	const ObjectStateBuf* getChildren() const ;
//	//	const ObjectStateBuf* prevBuf() const ;
//	//	const ObjectStateBuf* nextBuf() const;
//	//	void attachChild(ObjectStateBuf* child);
//	//	void detachChild(ObjectStateBuf* child);
//	//public:
//	//	template<typename T>
//	//	T* GetBuffer() const {
//	//		static_assert(false, "GetBuffer not implemented for the specified type!");
//	//		return (T*)0;
//	//	}
//	//	template<>
//	//	NodeBufImpl* GetBuffer<NodeBufImpl>() const {
//	//		return pNode;
//	//	}
//	//	template<>
//	//	RenderBufImpl* GetBuffer<RenderBufImpl>() const {
//	//		return pRImpl;
//	//	}
//	//	template<>
//	//	TransformBuf2DImpl* GetBuffer<TransformBuf2DImpl>() const {
//	//		return pTImpl;
//	//	}
//	//};
//	//
//	//ALIGN_AS(16) class ObjectStateBuf::NodeBufImpl : public AlignedNew<ObjectStateBuf::NodeBufImpl> {
//	//	typedef class ObjectStateBuf::NodeBufImpl* OSBNode;
//	//	friend class ObjectStateBuf::NodeBufImpl;
//	//protected:
//	//	OSBNode pParent;
//	//	OSBNode pFirstOwnChild;
//	//	OSBNode pNextChild;
//	//	OSBNode pPrevChild;
//	//	unsigned char currentUpdateBnum;
//	//public:
//	//	const ObjectStateBuf& owner;
//	//	BufType mType;
//	//	inline OSBNode getChildren() {
//	//		return pFirstOwnChild;
//	//	}
//	//	inline OSBNode getParent() {
//	//		return pParent;
//	//	}
//	//	inline OSBNode nextChild() {
//	//		return pNextChild;
//	//	}
//	//	inline OSBNode prevChild()	 {
//	//		return pPrevChild;
//	//	}
//	//	const inline ObjectStateBuf* getOwner() const{
//	//		return &owner;
//	//	}
//	//	inline void setParent(OSBNode parent) {
//	//		assert(parent);
//	//		pParent = parent;
//	//	};
//	//	inline void setNext(OSBNode next) {
//	//		assert(next);
//	//		pNextChild = next;
//	//	};
//	//	inline void setPrev(OSBNode prev) {
//	//		assert(prev);
//	//		pPrevChild = prev;
//	//	};
//	//	NodeBufImpl(const ObjectStateBuf& osb, const BufType type = BufType::NoBuf) 
//	//		: owner(osb),
//	//		mType(type),
//	//		currentUpdateBnum(0),
//	//		pParent(nullptr),
//	//		pFirstOwnChild(nullptr),
//	//		pPrevChild(nullptr),
//	//		pNextChild(nullptr)
//	//	{
//	//		assert(type != BufType::NoBuf);
//	//	}
//	//	void attachChild(OSBNode child) {
//	//		assert(child);
//	//		assert(!child->getParent());
//	//		assert(!child->prevChild());
//	//		assert(!child->nextChild());
//	//		child->setParent((OSBNode)this);
//	//		OSBNode pfoc = pFirstOwnChild;
//	//		pfoc->setPrev(child);
//	//		child->setNext(pfoc);
//	//		pfoc = child;
//	//	}
//	//	void detachChild(OSBNode child) {
//	//		assert(child);
//	//		assert(child->getParent() == (OSBNode)this);
//	//		if(child->prevChild()) {
//	//			child->prevChild()->setNext(child->nextChild());
//	//		}
//	//		if(child == pFirstOwnChild) {
//	//			pFirstOwnChild= child->nextChild();
//	//		}
//	//		if(child->nextChild()) {
//	//			child->nextChild()->setPrev(child->prevChild());
//	//		}
//	//		child->setNext(nullptr);
//	//		child->setPrev(nullptr);
//	//		child->setParent(nullptr);
//	//	}
//
//
//	//};
//
//	class OSB;
//	ALIGN_AS(16) class IBuffer : public AlignedNew<IBuffer> {
//		friend class OSB;
//	protected:
//		BufType mType;
//		OSB& mOwner;
//		void setType(const BufType type ) {
//			mType = type;
//		}
//	public:
//		const BufType getType() const {
//			return mType;
//		}
//		OSB& GetOwner() {
//			return mOwner;
//		}
//		IBuffer(OSB& owner, const BufType type) : mOwner(owner), mType(type) {};
//		//IBuffer(const IBuffer& other) :mOwner(other.mOwner), mType(other.mType) {};
//		//IBuffer(const IBuffer&& other) :mOwner(other.mOwner), mType(other.mType) {};
//		
//		virtual ~IBuffer() {};
//
//		virtual void Present() {};
//	};
//	typedef std::shared_ptr<IBuffer> IBufferPtr;
//
//	typedef std::unordered_map<TypeID, std::shared_ptr<Timewhale::IBuffer>> BufferMap;
//	ALIGN_AS(16) class OSB : public AlignedNew<OSB> {
//	private:
//		TypeID bufID;
//		BufferMap mBuffers;
//		struct {
//			OSB* pParent;
//			OSB* pFirstOwnChild;
//			OSB* pNextChild;
//			OSB* pPrevChild;
//		};
//	public: 
//		OSB(TypeID id) :bufID(id), pParent(0), pFirstOwnChild(0), pNextChild(0), pPrevChild(0)  {};
//		inline OSB* getChildren() {
//			return pFirstOwnChild;
//		}
//		inline OSB* getParent() {
//			return pParent;
//		}
//		inline OSB* nextChild() {
//			return pNextChild;
//		}
//		inline OSB* prevChild()	 {
//			return pPrevChild;
//		}
//		inline void setParent(OSB* parent) {
//			assert(parent);
//			pParent = parent;
//		};
//		inline void setNext(OSB* next) {
//			assert(next);
//			pNextChild = next;
//		};
//		inline void setPrev(OSB* prev) {
//			assert(prev);
//			pPrevChild = prev;
//		};
//		void attachChild(OSB* child) {
//			assert(child);
//			assert(!child->getParent());
//			assert(!child->prevChild());
//			assert(!child->nextChild());
//			child->setParent(this);
//			OSB* pfoc = pFirstOwnChild;
//			pfoc->setPrev(child);
//			child->setNext(pfoc);
//			pfoc = child;
//		}
//		void detachChild(OSB* child) {
//			assert(child);
//			assert(child->getParent() == this);
//			if(child->prevChild()) {
//				child->prevChild()->setNext(child->nextChild());
//			}
//			if(child == pFirstOwnChild) {
//				pFirstOwnChild= child->nextChild();
//			}
//			if(child->nextChild()) {
//				child->nextChild()->setPrev(child->prevChild());
//			}
//			child->setNext(nullptr);
//			child->setPrev(nullptr);
//			child->setParent(nullptr);
//		}
//
//		template<class T>
//		std::shared_ptr<T> GetBuffer() {
//			auto it = mBuffers.find(ctid<T>());
//			assert(it != mBuffers.end());
//			//return *it;
//			//BufferMap::iterator it = mBuffers.find(ctid<T>());
//			//assert(it != mBuffers.end());
//			//return it->second;
//			//IBufferPtr ibp = it->second;
//			return std::dynamic_pointer_cast<T>(it->second);
//		}
//		template<class T>
//		std::shared_ptr<T> CreateBuffer(const TypeID bufferComponentType) {
//			BRegMap& map = GetBufRegMap();
//			auto it = map.find(bufferComponentType);
//			assert(it != map.end());
//			std::shared_ptr<IBuffer> nb = it->second(*this);
//			auto ret = mBuffers.insert(std::make_pair(ctid<T>(), nb));
//			assert(!ret.second);
//			return std::dynamic_pointer_cast<T>(nb);
//		}
//		template<class T>
//		void DestroyBuffer() {
//			auto it = mBuffers.find(ctid<T>());
//			assert(it != mBuffers.end());
//			mBuffers.erase(it);
//		}
//	};
//}
//
//typedef std::shared_ptr<Timewhale::OSB> OSBPtr;
//
//
//
//	typedef std::shared_ptr<Timewhale::IBuffer> (*CreateBufferFunc)(const Timewhale::OSB&);
//	typedef std::unordered_map<TypeID, CreateBufferFunc> BRegMap;
//	
//	inline BRegMap& GetBufRegMap() {
//		static BRegMap reg;
//		return reg;
//	}
//	template<class T>
//	std::shared_ptr<Timewhale::IBuffer> CreateBuffer() {
//		return std::make_shared<T>();
//	}
//
//	template<class T>
//	struct BRegEntry {
//	public:
//		static BRegEntry<T>& Instance(const TypeID componentTypeID) {
//			static BRegEntry<T> instance(componentTypeID);
//			return instance;
//		}
//	private:
//		BRegEntry<T>(const TypeID typeID) {
//			BRegMap& map = GetBufRegMap();
//			CreateBufferFunc func = CreateBuffer<T>;
//			std::pair<BRegMap::iterator, bool> ret = 
//				map.insert(BRegMap::value_type(typeID, func));
//			if(!ret.second) {
//				assert(false)
//			}
//		}
//		BRegEntry(const BRegEntry<T>&);
//	};
//
//#define REGISTER_BUFFER(COMPONENT, BUFFER) \
//	namespace { \
//		static_assert(std::is_base_of<Timewhale::ECS::IComponent, COMPONENT>::value, "" STR(COMPONENT) " does not derive from IComponent and cannot have a buffer registered for it! " __FILE__":"STR(__LINE__)); \
//		static_assert(std::is_base_of<Timewhale::IBuffer, BUFFER>::value, "" STR(BUFFER) " does not derive from IBuffer and cannot be registered as a buffer for " STR(COMPONENT) "! " __FILE__":"STR(__LINE__)); \
//		template<class T> \
//		class BufferRegistration; \
//		template<> \
//		class BufferRegistration<BUFFER> { \
//			static const BRegEntry<BUFFER>& reg; \
//		}; \
//		const BRegEntry<BUFFER>& BufferRegistration<BUFFER>::reg = \
//			BRegEntry<BUFFER>::Instance(ctid<COMPONENT>()); \
//	};
//
//
//namespace Timewhale {
//	//template<typename T>
//	//struct has_dirty {
//	//	struct Fallback { bool dirty; };
//	//	struct Derived : T, Fallback {};
//
//	//	template<typename C, C> struct checkType;
//	//	template<typename C> static char f(checkType<bool Fallback::*, &C::dirty>*);
//	//	template<typename C> static uint32_t f(...);
//
//	//	static bool const value = sizeof(f<Derived>(0)) == sizeof(char);
//	//};
//	//template<typename T>
//	//struct has_owner {
//	//	struct Fallback{ ObjectStateBuf* owner; };
//	//	struct Derived : T, Fallback {};
//	//	template<typename C, C> struct checkType;
//	//	template<typename C> static char f(checkType<bool Fallback::*, &C::owner>*);
//	//	template<typename C> static uint32_t f(...);
//
//	//	static bool const value = sizeof(f<Derived>(0)) == sizeof(char);
//	//};
//	//template<typename T>
//	//struct _bufTypeCheck 
//	//	:std::integral_constant<bool,
//	//	has_owner<T>::value && has_dirty<T>::value>
//	//{};
//
//	//template<typename T, bool tcheck = /*_bufTypeCheck<T>::value*/true>
//	//struct _hasDirtyAncestor {
//	//	bool operator()(const T& start, ObjectStateBuf** first_dirty = nullptr, const size_t depth= 0) {
//	//		static_assert(false, "How did you get to this HasDirtyAncestor call? You broke cl.exe!");
//	//		return false;
//	//	}
//	//};
//	template<typename T>
//	struct _hasDirtyAncestor {
//		bool operator()(
//			T& start, 
//			OSB** first_dirty = nullptr, 
//			const size_t depth = 0) 
//		{
//			static_assert(std::is_base_of<Timewhale::IBuffer, T>::value, "Cannot create a _hasDirtyAncestor object. T is not a buffer! " __FILE__ ":" STR(__LINE__));
//			OSB *parent = &(start.GetOwner());
//			size_t i = depth;
//			//roll over i for effectively infinity
//			if(!depth) i = (size_t)(int)(-1);
//			while(parent && i > 0) {
//				std::shared_ptr<T> bufImpl = parent->GetBuffer<T>();
//				if(!bufImpl) return false; //parent with no buffer of this type
//				//the dirty check stops here
//				if(bufImpl->dirty) {
//					if(first_dirty) *first_dirty = const_cast<OSB*>(parent);
//					return true;
//				} 
//				parent = parent->getParent();
//				--i;
//			}
//			if(first_dirty) *first_dirty = nullptr;
//			return false;
//		}
//	};
//	//template<typename T>
//	//struct _hasDirtyAncestor<T, false> {
//	//	bool operator()(const T& start, ObjectStateBuf** first_dirty = nullptr, const size_t depth = 0) {
//	//		static_assert(false, "HasDirtyAncestor cannot be called on this Buffer type, as it does not have a \'dirty\' field");
//	//		return false;
//	//	}
//	//};
//	//Will check if the specified T buffer has a parent 
//	template<typename T>
//	static bool HasDirtyAncestor(T& start, OSB** first_dirty = nullptr) {
//		static _hasDirtyAncestor<T> ancestorGetter;
//		return ancestorGetter(start, first_dirty);
//	}
//	template<typename T>
//	static bool HasDirtyParent(const T& start) {
//		static _hasDirtyAncestor<T> ancestorGetter;
//		return ancestorGetter(start, nullptr, 1);
//	}

	
}

#endif