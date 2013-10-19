#include "Camera2D.h"
#include "SceneManager.h"
#include "DirectXMath.h"

namespace Timewhale {
	
	using DirectX;

	static Matrix4 Camera2D::GetWorldToScreen() 
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		return cam->WtoS;
	}

	static Matrix4 Camera2D::GetScreenToWorld()
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		return cam->StoW;
	}

	static Vector2 Camera2D::ToScreenCoords(const float world_x, const float world_y)
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		Vector2 world_pos(world_x, world_y);
		Vector2 res;
		XMStoreFloat4(
			XMVectorTransform(
				XMLoadFloat3(&world_pos),
				XMLoadFloat4x4(&cam->WtoS)),
			&res);
		return res;
	}
	static Vector2 Camera2D::ToWorldCoords(const float screen_x, const float screen_y)
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		Vector2 screen_pos(screen_x, screen_y);
		Vector2 res;
		XMStoreFloat2(
			XMVectorTransform(
				XMLoadFloat2(screen_pos),
				XMLoadFloat4x4(&cam->StoW)),
			&res);
		return res;
	}

	static void Camera2D::ToScreenCoords(
		const float world_x, const float world_y, float& screen_x, float& screen_y)
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		Vector2 world_pos(world_x, world_y);
		Vector2 res;
		XMStoreFloat2(
			XMVectorTransform(
				XMLoadFloat2(&world_pos),
				XMLoadFloat4x4(&cam->WtoS)),
			&res);
		screen_x = res.x;
		screen_y = res.y;
	}

	static void Camera2D::ToWorldCoords(
		const float screen_x, const float screen_y, float& world_x, float& world_y) 
	{
		auto cam = GetActiveSceneCamera();
		assert(cam);
		Vector2 screen_pos(screen_x, screen_y);
		Vector2 res;
		XMStoreFloat2(
			XMVectorTransform(
				XMLoadFloat2(screen_pos),
				XMLoadFloat4x4(&cam->StoW)),
			&res);
		world_x = res.x;
		world_y = res.y;
	}

	static Rectangle Camera2D::GetWorldBounds() {
		auto cam = GetActiveSceneCamera();
		assert(cam);
		return cam->world_bounds;
	}

	void Camera2D::GenerateTransforms() {
		if(!dirty) return;

		// Transform * RotationZ * Scaling * Translation
		XMStoreFloat4x4(
			XMMatrixMultiply(
				XMMatrixMultiply(
					XMMatrixMultiply(
						XMMatrixTransform(
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
					0.0f)),
			&WtoS);

		//StoW is easier...
		XMStoreFloat4x4(XMMatrixInverse(XMLoadFloat4x4(&WtoS)));

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

	inline Camera2D* GetActiveSceneCamera() {
		auto curr_scene = SceneManager::Current();
		assert(curr_scene);
		return curr_scene->GetCamera2D();
	}
	

}