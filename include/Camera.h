#pragma once
#ifndef __TIMEWHALE_CAMERA_H_
#define __TIMEWHALE_CAMERA_H_


#include "WhaleMath.h"

//undefine this to use the improved camera;
#define TW_OLDCAMERA


namespace Timewhale {

	class Camera {
	private:

#ifdef TW_OLDCAMERA
		Rectangle cameraTransform;
		float mScale;
#else
		float cam_x, cam_y;
		float cam_rot;
		float cam_scale;
		float cam_width, cam_height;

		bool dirty;

		Matrix4f WtoS;
		Matrix4f StoW;
		Rectangle screen_bounds;
		Rectangle world_bounds;
#endif
	public:
#ifdef TW_OLDCAMERA
		Camera(const float _x = 0.0f, const float _y = 0.0f, const float _w = 0.0f, const float _h = 0.0f, const float scale = 1.0f);
		void SetPosition(const float _x, const float _y);
		void Move(const float _x, const float _y);
		void SetBounds(const float _w, const float _h);
		void SetScale(const float scale);

		const float GetX() const;
		const float GetY() const;
		const float GetWidth() const;
		const float GetHeight() const;
		const void GetPosition(float& _x, float& _y) const;
		const void GetSize(float& _w, float& _h) const;
		const float GetScale() const;
		Rectangle& GetBounds();
#else

		Camera(
			const float x = 0.0f, 
			const float y = 0.0f, 
			const float width = 0.0f,
			const float height = 0.0f,
			const float scale = 0.0f,
			const float rotation = 0.0f)
				:cam_x(x), cam_y(y),
				cam_width(width), cam_height(height),
				cam_rot(rotation),
				cam_scale(scale),
				WtoS(), StoW(),
				screen_bounds(),world_bounds(),
				dirty(false)
		{
		}

		void SetPosition(const float x, const float y);
		void Move(const float x, const float y);
		void SetRotation(const float rot);
		void Rotate(const float rot);
		void SetScale(const float scale);
		void ScaleBy(const float scale);
		void SetSize(const float w, const float h);

#define SetBounds(x, y) SetSize(x, y)
		


		void GetPosition(float& x, float &y) const;
		const float GetX() const;
		const float GetY() const;
		const float GetRotation() const;
		void GetSize(float& w, float& h) const;
		const float GetWidth() const;
		const float GetHeight() const;
		const float GetScale() const;
		const Rectangle GetBounds() const;

		static Matrix4f GetWorldToScreen();
		static Matrix4f GetScreenToWorld();

		static Vector2f ToScreenCoords(const float world_x, const float world_y);
		static Vector2f ToWorldCoords(const float screen_x, const float screen_y);

		static void ToScreenCoords(
			const float world_x, const float world_y, float& screen_x, float& screen_y);

		static void ToWorldCoords(
			const float screen_x, const float screen_y, float& world_x, float& world_y);

		static Rectangle GetWorldBounds();

	private:
		void GenerateTransforms();
		
#endif

	};

}

#endif