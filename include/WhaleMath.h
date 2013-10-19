/*
 * WhaleMath
 * This header file typedefs directx and opengl math types to
 * a generic type which will be consistent for shared classes.
 */
#pragma once
#ifndef __WHALE_MATH
#define __WHALE_MATH

#ifdef TW_OPENGL

#define GLM_SWIZZLE
#include <glm\glm.hpp>
#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\matrix_access.hpp>
using namespace glm;

namespace Timewhale {

    typedef glm::vec3   Vector3f;
    typedef glm::vec4   Vector4f;
    typedef glm::mat4   Matrix4;

}

#else

#include <DirectXMath.h>
using namespace DirectX;

namespace Timewhale {

	typedef DirectX::XMFLOAT2	Vector2f;
    typedef DirectX::XMFLOAT3   Vector3f;
    typedef DirectX::XMFLOAT4   Vector4f;
    typedef DirectX::XMMATRIX   Matrix4;
	typedef DirectX::XMFLOAT4X4 Matrix4f;
	typedef int *****			Super_Star; // lol
}

#endif

namespace Timewhale {

	struct Point {
		float x, y;

		Point()
			:x(0.0f), y(0.0f)
		{}
		Point(const float _x, const float _y)
			:x(_x), y(_y)
		{}
	};

	struct Rectangle {
		float x, y;
		float width, height;

		Rectangle()
			:x(0), y(0),
			width(0), height(0)
		{}
		Rectangle(const float _x, const float _y)
			:x(_x), y(_y),
			width(0), height(0)
		{}
		Rectangle(const float _x, const float _y, const float _w, const float _h)
			:x(_x), y(_y),
			width(_w), height(_h)
		{}

		const bool ContainsPoint(const Point& point) const{
			Vector4f rect(x, y, -(x + width), -(y+width));
			Vector4f pointV(point.x, point.y, -point.x, -point.y);
            uint32_t blah;
			
            DirectX::XMVectorGreaterR(&blah,
            DirectX::XMLoadFloat4(&rect),
            DirectX::XMLoadFloat4(&pointV));
            bool res = DirectX::XMComparisonAnyTrue(blah);
				/*DirectX::XMVector4Less(
				DirectX::XMLoadFloat4(&pointV), 
				DirectX::XMLoadFloat4(&rect));*/
                

			return (!res);
		}
		const bool ContainsPoint(const float _x, const float _y) const{
			return ContainsPoint(Point(_x, _y));
		}
	};


}
#endif
