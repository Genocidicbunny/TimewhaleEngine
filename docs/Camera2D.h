#pragma once
#ifndef __TIMEWHALE_CAMERA2D_H_
#define __TIMEWHALE_CAMERA2D_H_
#include "WhaleMath.h"


namespace Timewhale {
	
	class Camera2D {
		float cam_x, cam_y;
		float cam_rot;
		float cam_scale;

		float cam_width, cam_height;

		bool dirty;

		Matrix4 WtoS;
		Matrix4 StoW;
		Rectangle screen_bounds;
		Rectangle world_bounds;

	public:
		Camera2D(
			const float width, 
			const float height,
			const float x = 0.0f, 
			const float y = 0.0f, 
			const float rotation = 0.0f,
			const float scale = 1.0f) 
				:cam_x(x), cam_y(y),
				cam_rot(rotation),
				cam_scale(scale),
				cam_width(width), cam_height(height),
				dirty(true)
		{}

		inline void SetPosition(const float x, const float y) {
			cam_x = x;
			cam_y = y;
			dirty = true;
		}
		inline void Move(const float x, const float y) {
			cam_x += x;
			cam_y += y;
			dirty = true;
		}
		inline void SetRotation(const float rot) {
			cam_rot = rot;
			dirty = true;
		}
		inline void Rotate(const float rot) {
			cam_rot += rot;
			dirty = true;
		}
		inline void SetScale(const float scale) {
			cam_scale = scale;
			dirty = true;
		} 
		inline void ScaleBy(const float scale) {
			cam_scale += scale;
			dirty = true;
		}
		inline void SetSize(const float w, const float h) {
			cam_width = w;
			cam_height = h;
			dirty = true;
		}

		inline void GetPosition(float& x, float &y) const {
			x = cam_x;
			y = cam_y;
			dirty = true;
		}


		inline const float GetX() const {
			return cam_x;
		}
		inline const float GetY() const {
			return cam_y;
		}
		inline const float GetRotation() const {
			return cam_rot;
		} 
		inline void GetSize(float& w, float& h) const {
			w = cam_width;
			h = cam_height;
		}
		inline const float GetWidth() const {
			return cam_width;
		}
		inline const float Getheight() const {
			return cam_height;
		}
		inline const float GetScale() const {
			return cam_scale;
		}
		inline const Rectangle GetBounds() const {
			return Rectangle(cam_x, cam_y, cam_width, cam_height);
		}

		static Matrix4 GetWorldToScreen();
		static Matrix4 GetScreenToWorld();

		static Vector2 ToScreenCoords(const float world_x, const float world_y);
		static Vector2 ToWorldCoords(const float screen_x, const float screen_y);

		static void ToScreenCoords(
			const float world_x, const float world_y, float& screen_x, float& screen_y);

		static void ToWorldCoords(
			const float screen_x, const float screen_y, float& world_x, float& world_y);

		static Rectangle GetWorldBounds();

	private:
		void GenerateTransforms();

	}

}


#endif