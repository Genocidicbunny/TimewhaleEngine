#include "Camera.h"

#ifdef TW_OLDCAMERA

namespace Timewhale {

	Camera::Camera(const float _x, const float _y, const float _w, const float _h, const float scale)
		:cameraTransform(_x, _y, _w, _h), mScale(scale)
	{}
	void Camera::SetPosition(const float _x, const float _y) {
		cameraTransform.x = _x;
		cameraTransform.y = _y;
	}
	void Camera::Move(const float _x, const float _y) {
		cameraTransform.x += _x;
		cameraTransform.y += _y;
	}
	void Camera::SetBounds(const float _w, const float _h) {
		cameraTransform.width = _w;
		cameraTransform.height = _h;
	}
	void Camera::SetScale(const float scale) {
		mScale = scale;
	}
	const float Camera::GetX() const {
		return cameraTransform.x;
	}
	const float Camera::GetY() const {
		return cameraTransform.y;
	}
	const float Camera::GetWidth() const {
		return cameraTransform.width;
	}
	const float Camera::GetHeight() const {
		return cameraTransform.height;
	}
	const void Camera::GetPosition(float& _x, float& _y) const {
		_x = cameraTransform.x;
		_y = cameraTransform.y;
	}
	const void Camera::GetSize(float& _w, float& _h) const {
		_w = cameraTransform.width;
		_h = cameraTransform.height;
	}
	const float Camera::GetScale() const {
		return mScale;
	}
	Rectangle& Camera::GetBounds() {
		return cameraTransform;
	}
}
#else

#include "SceneManager.h"
#include "DirectXMath.h"

using namespace DirectX;

namespace Timewhale {

	void Camera::SetPosition(const float x, const float y) {
		cam_x = x;
		cam_y = y;
		dirty = true;
	}
	void Camera::Move(const float x, const float y) {
		cam_x += x;
		cam_y += y;
		dirty = true;
	}
	void Camera::SetRotation(const float rot) {
		cam_rot = rot;
		dirty = true;
	}
	void Camera::Rotate(const float rot) {
		cam_rot += rot;
		dirty = true;
	}
	void Camera::SetScale(const float scale) {
		cam_scale = scale;
		dirty = true;
	} 
	void Camera::ScaleBy(const float scale) {
		cam_scale += scale;
		dirty = true;
	}
	void Camera::SetSize(const float w, const float h) {
		cam_width = w;
		cam_height = h;
		dirty = true;
	}

	void Camera::GetPosition(float& x, float &y) const {
		x = cam_x;
		y = cam_y;
	}
	const float Camera::GetX() const {
		return cam_x;
	}
	const float Camera::GetY() const {
		return cam_y;
	}
	const float Camera::GetRotation() const {
		return cam_rot;
	} 
	void Camera::GetSize(float& w, float& h) const {
		w = cam_width;
		h = cam_height;
	}
	const float Camera::GetWidth() const {
		return cam_width;
	}
	const float Camera::GetHeight() const {
		return cam_height;
	}
	const float Camera::GetScale() const {
		return cam_scale;
	}
	const Rectangle Camera::GetBounds() const {
		return Rectangle(cam_x, cam_y, cam_width, cam_height);
	}
	
	
	inline Camera* GetActiveSceneCamera() {
		auto curr_scene = SceneManager::GetCurrentScene();
		assert(curr_scene);
		return curr_scene->GetCamera();
	}
	
	Matrix4f Camera::GetWorldToScreen() 
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		return cam->WtoS;
	}

	Matrix4f Camera::GetScreenToWorld()
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		return cam->StoW;
	}

	Vector2f Camera::ToScreenCoords(const float world_x, const float world_y)
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		Vector2f world_pos(world_x, world_y);
		Vector2f res;
		XMStoreFloat2(
			&res,
			XMVector2Transform(
				XMLoadFloat2(&world_pos),
				XMLoadFloat4x4(&cam->WtoS)));
		return res;
	}
	Vector2f Camera::ToWorldCoords(const float screen_x, const float screen_y)
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		Vector2f screen_pos(screen_x, screen_y);
		Vector2f res;
		XMStoreFloat2(
			&res,
			XMVector2Transform(
				XMLoadFloat2(&screen_pos),
				XMLoadFloat4x4(&cam->StoW)));
		return res;
	}

	void Camera::ToScreenCoords(
		const float world_x, const float world_y, float& screen_x, float& screen_y)
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		Vector2f world_pos(world_x, world_y);
		Vector2f res;
		XMStoreFloat2(
			&res,
			XMVector2Transform(
				XMLoadFloat2(&world_pos),
				XMLoadFloat4x4(&cam->WtoS)));
		screen_x = res.x;
		screen_y = res.y;
	}

	void Camera::ToWorldCoords(
		const float screen_x, const float screen_y, float& world_x, float& world_y) 
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		Vector2f screen_pos(screen_x, screen_y);
		Vector2f res;
		XMStoreFloat2(
			&res,
			XMVector2Transform(
				XMLoadFloat2(&screen_pos),
				XMLoadFloat4x4(&cam->StoW)));
		world_x = res.x;
		world_y = res.y;
	}

	Rectangle Camera::GetWorldBounds() {
		auto cam = GetActiveSceneCamera();
		assert(cam);
		return cam->world_bounds;
	}

	void Camera::GenerateTransforms() {
		if(!dirty) return;

		// Transform * RotationZ * Scaling * Translation
		XMStoreFloat4x4(
			&WtoS,
			XMMatrixMultiply(
				XMMatrixMultiply(
					XMMatrixMultiply(
						XMMatrixTranslation(
							-cam_x,
							-cam_y,
							0),
						XMMatrixRotationZ(
							cam_rot)),
					XMMatrixScaling(
						cam_scale,
						cam_scale,
						1.0f)),
				XMMatrixTranslation(
					cam_width * 0.5f,
					cam_height * 0.5f,
					0.0f)
				)
			);

		//StoW is easier...
		XMVECTOR det;
		XMStoreFloat4x4(&StoW, XMMatrixInverse(&det, XMLoadFloat4x4(&WtoS)));

		//Bounds. These wont make much sense under rotation
		screen_bounds.x = cam_x;
		screen_bounds.y = cam_y;
		screen_bounds.width = cam_width;
		screen_bounds.height = cam_height;


		//Now for the fun one!
		float lo_x, lo_y, ur_x, ur_y;
		ToWorldCoords(cam_x, cam_y, lo_x, lo_y);
		ToWorldCoords(cam_x + cam_width, cam_y + cam_height, ur_x, ur_y);

		world_bounds.x = lo_x;
		world_bounds.y = lo_y;
		world_bounds.width = ur_x - lo_x;
		world_bounds.height = ur_y - lo_y;
	}

	
}
#endif

