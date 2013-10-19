//#include "ECS\ObjectState.h"
//#include <cassert>
//#include "Rendering\RenderBuffers.h"
//#include "ECS\TransformBuffers.h"
//
//namespace Timewhale {
////#define BALEEN_LOCK(inst) std::lock_guard<std::mutex> lock_guard((inst).baleenMtx)
////
////	bool Baleen::get_oldest_for_update() {
////		Baleen& instance = GetBaleen();
////		BALEEN_LOCK(instance);
////		assert(instance.bufListHead);
////		bufNode* mover;
////		for(mover = instance.bufListHead; mover != NULL || mover->s_lock._r_lock; mover = mover->next);
////		assert(mover); //Mover CANNOT be null here. If it is, something has gone seriously seriously wrong
////		instance.unlink_node(mover);
////		bool lockres = mover->s_lock.get_lock(bufWriteLock);
////		assert(lockres); //Again, this CANNOT fail for normal operation
////		instance.updateNode = mover;
////		//instance.updateableNode_w = instance.updateNode->s_offset;
////		bool lockres2 = instance.bufListHead->s_lock.get_lock(bufReadLock);
////		assert(lockres2);
////		//instance.updateableNode_r = instance.bufListHead->s_offset;
////		return true;
////	}
////
////	bool Baleen::get_oldest_for_render() {
////		Baleen& instance = GetBaleen();
////		BALEEN_LOCK(instance);
////		assert(instance.renderNode);
////		if(!instance.renderNode->prev || instance.renderNode->prev->s_lock._w_lock)
////			return false;
////		instance.renderNode->s_lock.release_lock(bufReadLock);
////		instance.renderNode = instance.renderNode->prev;
////		assert(instance.renderNode);
////		bool lockres = instance.renderNode->s_lock.get_lock(bufReadLock);
////		assert(lockres);
////		return true;
////		
////	}
////
////	bool Baleen::finalize_update(const unsigned long tick) {
////		Baleen& instance = GetBaleen();
////		BALEEN_LOCK(instance);
////		assert(instance.updateNode);
////		assert(instance.updateNode->tickNum < tick);
////		instance.updateNode->tickNum = tick;
////		instance.updateNode->s_lock.release_lock(bufWriteLock);
////		instance.push_front(instance.updateNode);
////		instance.updateNode = nullptr;
////		return true;
////	}
////
////	bool Baleen::finalize_render() {
////		Baleen& instance = GetBaleen();
////		BALEEN_LOCK(instance);
////		assert(instance.renderNode);
////		instance.renderNode->s_lock.release_lock(bufReadLock);
////		//dont reset renderNode, we need it for the next render pass.
////		return true;
////
////	}
////	void Baleen::unlink_node(bufNode* node) {
////		if(!node) return;
////		BALEEN_LOCK(*this);
////		if(node->prev) {
////			node->prev->next = node->next;
////		}
////		if(node == bufListHead) {
////			assert(node->next);
////			bufListHead = node->next;
////		}
////		if(node->next) {
////			node->next->prev = node->prev;
////		}
////				
////	}
////	void Baleen::push_front(bufNode* node) {
////		if(!node) return;
////		BALEEN_LOCK(*this);
////		assert(bufListHead);
////		bufListHead->prev = node;
////		node->next = bufListHead;
////		bufListHead = node;
////	}
////#ifdef BALEEN_LOCK
////#undef BALEEN_LOCK
////#endif
//	
//	ObjectStateBuf::ObjectStateBuf(const unsigned int id, const BufType RenderType, const BufType TransformType, ObjectStateBuf *parent) 
//		: bufID(id),
//		pRImpl(/*new RenderBufImpl(*this, RenderType)*/ nullptr),
//		pTImpl(new TransformBuf2DImpl(*this, TransformType)),
//		pNode(nullptr)
//	{
//		if(parent) {
//			pNode = new NodeBufImpl(*this, TransformType);
//			parent->attachChild(this);
//		}
//		
//	}
//	const ObjectStateBuf* ObjectStateBuf::getParent() const{
//		if(!isNode()) return nullptr;
//		if(!pNode->getParent()) return nullptr;
//		return pNode->getParent()->getOwner();
//	}
//	const ObjectStateBuf* ObjectStateBuf::getChildren()  const{
//		if(!isNode() || pNode->getChildren()) return nullptr;
//		return pNode->getChildren()->getOwner();
//	}
//	const ObjectStateBuf* ObjectStateBuf::prevBuf()  const{
//		if(!isNode() || pNode->prevChild()) return nullptr;
//		return pNode->prevChild()->getOwner();
//	}
//	const ObjectStateBuf* ObjectStateBuf::nextBuf()  const{
//		if(!isNode() || pNode->nextChild()) return nullptr;
//		return pNode->nextChild()->getOwner();
//	}
//	void ObjectStateBuf::attachChild(ObjectStateBuf* child) {
//		assert(child);
//		assert(child->pNode);
//		if(!isNode()) AddNode();
//		assert(pNode);
//		pNode->attachChild(child->pNode);
//	}
//	void ObjectStateBuf::detachChild(ObjectStateBuf* child) {
//		assert(child);
//		assert(child->pNode);
//		assert(isNode());
//		pNode->detachChild(child->pNode);
//	}
//	void ObjectStateBuf::AddNode() {
//		assert(!pNode);
//		pNode = new NodeBufImpl(*this, pTImpl ? pTImpl->mType : BufType::StaticBuf);
//	}
//	void ObjectStateBuf::AddRenderBuffer(const BufType type) {
//		if(!pRImpl && pTImpl) 
//			pRImpl = new RenderBufImpl(*this, type);
//	}
//	void ObjectStateBuf::AddTransformBuffer(const BufType type) {
//		if(!pTImpl) 
//			pTImpl = new TransformBuf2DImpl(*this, type);
//	}
//	const bool ObjectStateBuf::isNode() const {
//		return pNode != nullptr;
//	}
//	/*inline void ObjectStateBuf::setRenderBuffer(const unsigned char writeBuf, const unsigned char readBuf) {
//		if(!pRImpl) return;
//		assert(writeBuf < 3);
//		assert(readBuf < 3);
//		assert(writeBuf != readBuf);
//		pRImpl->setBuffer(writeBuf, readBuf);
//	}
//	inline void ObjectStateBuf::setTransformBuffers(const unsigned char writeBuf, const unsigned char readBuf) {
//		if(!pTImpl) return;
//		assert(writeBuf < 3);
//		assert(readBuf < 3);
//		assert(writeBuf != readBuf);
//		pTImpl->setBuffer(writeBuf, readBuf);
//	}
//	inline void ObjectStateBuf::setNodeBuffers(const unsigned char writeBuf, const unsigned char readBuf) {
//		if(!isNode()) return;
//		assert(writeBuf < 3);
//		assert(readBuf < 3);
//		assert(writeBuf != readBuf);
//		pNode->setBuffer(writeBuf, readBuf);
//	}*/
//
//}