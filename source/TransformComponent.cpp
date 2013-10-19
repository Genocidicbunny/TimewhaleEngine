#include "TransformComponent.h"

namespace Timewhale {
	Transform2D::Transform2D()
		:IComponent(),
		mTransform(),
		mBounds(),
		rotationRadians(0.0f),
		scaleFactorX(1.0f),
		scaleFactorY(1.0f)
	{}
	void Transform2D::SetPosition(const float x, const float y) {
		mTransform.x = x;
		mTransform.y = y;
		dirty = true;
	}
	void Transform2D::Move(const float _x, const float _y) {
		mTransform.x += _x;
		mTransform.y += _y;
		dirty = true;
	}
	void Transform2D::SetScale(const float _sX, const float _sY) {
		scaleFactorX = _sX;
		if(_sY == 0.0f) scaleFactorY = _sX;
		else scaleFactorY = _sY;
		dirty = true;
	}
	void Transform2D::SetSize(const float _w, const float _h) {
		mTransform.width = _w;
		mTransform.height = _h;
		dirty = true;
	}
	void Transform2D::ResetScale() {
		scaleFactorX = 1.0f;
		scaleFactorY = 1.0f;
		dirty = true;
	}
	void Transform2D::SetRotation(const float _r) {
		rotationRadians = _r;
		dirty = true;
	}
	void Transform2D::Rotate(const float _r) {
		rotationRadians += _r;
		dirty = true;
	}

	void Transform2D::Update() {
		if(dirty) {
			mBounds.x = mTransform.x;
			mBounds.y = mTransform.y;
			mBounds.width = mTransform.width * scaleFactorX;
			mBounds.height = mTransform.height * scaleFactorY;
			dirty = false;
		}
	}
	bool Transform2D::IsDirty() {
		return dirty;
	}

	const float Transform2D::GetX() const {
		return mTransform.x;
	}
	const float Transform2D::GetY() const {
		return mTransform.y;
	}
	const void  Transform2D::GetPosition(float& _x, float& _y) const {
		_x = mTransform.x;
		_y = mTransform.y;
	}
	const float Transform2D::GetScaleX() const {
		return scaleFactorX;
	}
	const float Transform2D::GetScaleY() const {
		return scaleFactorY;
	}
	const void Transform2D::GetScale(float& _sX, float& _sY) const {
		_sX = scaleFactorX;
		_sY = scaleFactorY;
	}
	const float Transform2D::GetWidth() const {
		return mTransform.width;
	}
	const float Transform2D::GetHeight() const {
		return mTransform.height;
	}
	const void Transform2D::GetSize(float& _w, float& _h) const {
		_w = mTransform.width;
		_h = mTransform.height;
	}
	float Transform2D::GetScaledWidth() {
		if(IsDirty()) Update(); 
		return mBounds.width;
	}
	float Transform2D::GetScaledHeight() {
		if(IsDirty()) Update(); 
		return mBounds.height;
	}
	void Transform2D::GetScaledSize(float& _w, float& _h) {
		if(IsDirty()) Update(); 
		_w = mBounds.width;
		_h = mBounds.height;
	}
	const float Transform2D::GetRotation() const {
		return rotationRadians;
	}
	Rectangle& Transform2D::GetBoundsRect() {
		if(IsDirty()) Update();
		return mBounds;
	}
}