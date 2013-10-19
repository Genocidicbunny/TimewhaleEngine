#pragma once
#ifndef _UISHADER_H_
#define _UISHADER_H_

#include <d3d11.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include <fstream>

using namespace std;
using namespace DirectX;

namespace Timewhale {

class UIShader
{
private:
	struct ConstantBufferType
	{
		XMMATRIX projection;
		XMFLOAT2 screenWH;
		XMFLOAT2 translateXY;
	};

public:
	UIShader();
	UIShader(const UIShader&);
	~UIShader();

	bool Initialize();
	void Shutdown();
	bool Render(int indexCount, XMMATRIX& projectionMatrix, int screenWidth, int screenHeight, int translateX, 
					  int translateY, ID3D11ShaderResourceView* texture);

	void EnableScissor();
	void DisableScissor();

private:
	bool InitializeShader(WCHAR* vsFilename, WCHAR* psFilename);
	void ShutdownShader();

	bool SetShaderParameters(XMMATRIX& projectionMatrix, int screenWidth, int screenHeight, int translateX, int translateY, ID3D11ShaderResourceView* texture);
	void RenderShader(int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;

	// Scissoring States
	ID3D11RasterizerState* m_ScissorEnable;
	ID3D11RasterizerState* m_ScissorDisable;
};

}

#endif