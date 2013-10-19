#pragma once
#ifndef _UIOBJECT_H_
#define _UIOBJECT_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include "Rocket\Core.h"

using namespace DirectX;
namespace Timewhale {

struct TextureDetails;

class UIObject
{
private:
	struct VertexType
	{
		XMFLOAT3 position;
		XMFLOAT4 color;
		XMFLOAT2 texture;
	};

public:
	UIObject();
	UIObject(const UIObject&);
	~UIObject();

	bool Initialize(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, uint16_t textureID, TextureDetails tex);
	void Shutdown();
	void Render();

	int GetIndexCount();
	uint16_t GetTextureID();

private:
	bool InitializeBuffers(Rocket::Core::Vertex* rocketVertices, int num_vertices, int* rocketIndices, int num_indices, TextureDetails tex);
	void ShutdownBuffers();
	void RenderBuffers();

private:
	ID3D11Buffer *m_vertexBuffer, *m_indexBuffer;
	int m_vertexCount, m_indexCount;

	uint16_t m_textureID;
};

}

#endif