#pragma once
#ifndef _SPRITESHADER_H_
#define _SPRITESHADER_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>

using namespace std;
using namespace DirectX;

namespace Timewhale {

class SpriteShader
{
private:
	struct ConstantBufferType
	{
		XMMATRIX projection;
		XMFLOAT4 screenWHcameraXY;
		float scale;
	};

public:
	SpriteShader();
	SpriteShader(const SpriteShader&);
	~SpriteShader();

	bool Initialize();
	void Shutdown();
	bool Render(int vertexCount, int instanceCount, XMMATRIX& projectionMatrix, int screenWidth, int screenHeight, 
		int cameraX, int cameraY, ID3D11ShaderResourceView* texture, float scale);

private:
	bool InitializeShader(WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();

	bool SetShaderParameters(XMMATRIX& projectionMatrix, int screenWidth, int screenHeight, int cameraX, int cameraY, 
		ID3D11ShaderResourceView* texture, float scale);
	void RenderShader(int vertexCount, int instanceCount);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

}

#endif