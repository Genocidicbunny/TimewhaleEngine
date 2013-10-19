#pragma once
#ifndef _DXTEXTURE_H_
#define _DXTEXTURE_H_

#include "RenderSystem.h"
#include "WICTextureLoader.h"

using namespace DirectX;
namespace Timewhale {

class DXTexture
{
public:
	DXTexture();
	DXTexture(const DXTexture&);
	~DXTexture();

	bool Initialize(uint8_t* data, uint32_t len, uint32_t& width, uint32_t& height);
	bool Initialize(ID3D11Texture2D* texture);
	void Shutdown();

	ID3D11ShaderResourceView* GetTexture();

private:
	ID3D11Resource* m_resource;
	ID3D11ShaderResourceView* m_texture;
};

}

#endif