#pragma once
#ifndef _D3DMANAGER_H_
#define _D3DMANAGER_H_

#include <dxgi.h>
#include <d3dcommon.h>
#include <d3d11.h>
#include <DirectXMath.h>
#include <DXGI1_2.h>
#include <d3d11sdklayers.h>

using namespace DirectX;
using namespace std;
namespace Timewhale {

class D3DManager
{
public:
        D3DManager(void);
        ~D3DManager(void);

        bool Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, float screenDepth, float screenNear);
		bool Resize(int screenWidth, int screenHeight, float screenDepth, float screenNear);
        void Shutdown();
        
        void BeginScene(float red, float green, float blue, float alpha);
        void EndScene();

        ID3D11Device* GetDevice();
        ID3D11DeviceContext* GetDeviceContext();

		void GetProjectionMatrix(XMMATRIX& projectionMatrix);
        void GetWorldMatrix(XMMATRIX& worldMatrix);
		void GetOrthoMatrix(XMMATRIX& orthoMatrix);
		void GetViewMatrix(XMMATRIX& viewMatrix);

        void GetVideoCardInfo(char* cardName, int& memory);

        int GetScreenWidth();
        int GetScreenHeight();

        void TurnZBufferOn();
        void TurnZBufferOff();

        void TurnOnAlphaBlending();
        void TurnOffAlphaBlending();

private:
		void ErrorDescription(HRESULT hr);

        bool m_vsync_enabled;
        int m_videoCardMemory;
        char m_videoCardDescription[128];
        IDXGISwapChain* m_swapChain;
        ID3D11Device* m_device;
        ID3D11DeviceContext* m_deviceContext;
        ID3D11RenderTargetView* m_renderTargetView;
        ID3D11Texture2D* m_depthStencilBuffer;
        ID3D11DepthStencilState* m_depthStencilState;
        ID3D11DepthStencilView* m_depthStencilView;
        ID3D11RasterizerState* m_rasterState;
#ifdef _DEBUG
		ID3D11Debug* m_Debug;
#endif
		XMFLOAT4X4 m_projectionMatrix;
		XMFLOAT4X4 m_worldMatrix;
		XMFLOAT4X4 m_orthoMatrix;
		XMFLOAT4X4 m_viewMatrix;
        int m_ScreenWidth, m_ScreenHeight;

        ID3D11DepthStencilState* m_depthDisabledStencilState;

        ID3D11BlendState* m_alphaEnableBlendingState;
        ID3D11BlendState* m_alphaDisableBlendingState;
};

}

#endif