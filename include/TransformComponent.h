#pragma once
#ifndef __TIMEWHALE_TRANSFORM2D_H_
#define __TIMEWHALE_TRANSFORM2D_H_
#include "ECS.h"
#include "WhaleMath.h"

namespace Timewhale {

	class Transform2D : public IComponent {
	private:
		Rectangle mTransform;
		Rectangle mBounds;
		float rotationRadians;
		float scaleFactorX;
		float scaleFactorY;
		bool dirty;
	public:
		Transform2D();
		void SetPosition(const float x, const float y);
		void Move(const float _x, const float _y);
		void SetScale(const float _sX, const float _sY = 0.0f);
		void SetSize(const float _w, const float _h);
		void ResetScale();
		void SetRotation(const float _r);
		void Rotate(const float _r);

		//Integer-based functions that do autocasts to floats
		/*inline void SetPosition(const int x, const int y)
		{
			SetPosition(float(x), float(y));
		}
		inline void Move(const int x, const int y)
		{
			Move(float(x), float(y));
		}
		inline void SetPosition(const double x, const double y)
		{
			SetPosition(float(x), float(y));
		}
		inline void Move(const double x, const double y)
		{
			Move(float(x), float(y));
		}*/
		template<typename N>
		void SetSize(const N x, const N y) {
			SetSize(float(x), float(y));
		}
		template<typename N>
		void SetPosition(const N x, const N y) {
			SetPosition(float(x), float(y));
		}
		template<typename N>
		void Move(const N x, const N y) {
			Move(float(x), float(y));
		}


		
		virtual void Update();
		bool IsDirty();

		const float GetX() const;
		const float GetY() const;
		const void  GetPosition(float& _x, float& _y) const;
		const float GetScaleX() const;
		const float GetScaleY() const;
		const void  GetScale(float& _sX, float& _sY) const;
		const float GetWidth() const;
		const float GetHeight() const;
		const void  GetSize(float& _w, float& _h) const;
		float GetScaledWidth() ;
		float GetScaledHeight() ;
		void  GetScaledSize(float& _w, float& _h) ;
		const float GetRotation() const;
		Rectangle& GetBoundsRect();
	};

	REGISTER_COMPONENT_WFLAGS(Transform2D, "Transform2D", CREG_FLAGS_ENGCOMP);
	static const TypeID TransformTypeID(ctid<Transform2D>());

	typedef std::shared_ptr<Transform2D> Transform2DPtr;
}

#endif