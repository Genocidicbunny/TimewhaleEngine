#pragma once
#ifndef _SPRITE_H_
#define _SPRITE_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <vector>

using namespace DirectX;
namespace Timewhale {

struct SpriteInstanceData {
	// Stores team color info
	XMFLOAT3 color;
	// Stores the u and v coordinates for texturing
	XMFLOAT4 uv;
	// Stores the x position, y position, width and height of the sprite
	XMFLOAT4 xywh;

	SpriteInstanceData() : color(XMFLOAT3(0.0f, 0.0f, 0.0f)), xywh(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)), uv(XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)) {}
	SpriteInstanceData(XMFLOAT4 data, XMFLOAT4 data2) : color(XMFLOAT3(0.0f, 0.0f, 0.0f)), xywh(data), uv(data2) {}
	SpriteInstanceData(XMFLOAT3 color, XMFLOAT4 data, XMFLOAT4 data2) : color(color), xywh(data), uv(data2) {}
    SpriteInstanceData(const SpriteInstanceData &other) { *this = other; }
    SpriteInstanceData &operator= (const SpriteInstanceData &other) {
        if (this == &other) return *this;
        color = other.color;
        uv = other.uv;
        xywh = other.xywh;
		return *this;
    }
};

class SpriteBatch
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
	};

public:
	SpriteBatch();
	SpriteBatch(const SpriteBatch&);
	~SpriteBatch();

	bool Initialize();
	void Shutdown();
	void Render();

	int GetVertexCount();
	int GetInstanceCount();

	bool UpdateBuffers(std::vector<SpriteInstanceData>);

private:
	bool InitializeBuffers();
	void ShutdownBuffers();
	void RenderBuffers();

private:
	ID3D11Buffer *m_vertexBuffer, *m_instanceBuffer;
	int m_vertexCount, m_instanceCount;
};

}

#endif