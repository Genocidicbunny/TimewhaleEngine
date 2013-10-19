#include "DXTexture.h"

namespace Timewhale {

DXTexture::DXTexture()
{
	m_texture = 0;
	m_resource = 0;
}

DXTexture::DXTexture(const DXTexture& tex)
{
}

DXTexture::~DXTexture()
{
}

bool DXTexture::Initialize(uint8_t* data, uint32_t len, uint32_t& width, uint32_t& height)
{
	auto renderer = RenderSystem::get();

	HRESULT result;
	result = CreateWICTextureFromMemory(renderer->mRenderer->GetDevice(), NULL, data, len, &m_resource, &m_texture);
	if(FAILED(result))
	{
		return false;
	}

	ID3D11Texture2D* texResource = (reinterpret_cast<ID3D11Texture2D*>(m_resource));
	D3D11_TEXTURE2D_DESC desc;
	texResource->GetDesc(&desc);

	width = desc.Width;
	height = desc.Height;

	return true;
}

bool DXTexture::Initialize(ID3D11Texture2D* texture) 
{
	auto renderer = RenderSystem::get();

	HRESULT result;
	D3D11_TEXTURE2D_DESC desc;
	//D3D11_SHADER_RESOURCE_VIEW_DESC viewDesc;

	texture->GetDesc(&desc);

	//viewDesc.Format = desc.Format;
	//viewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	result = renderer->mRenderer->GetDevice()->CreateShaderResourceView(texture, NULL, &m_texture);
	if(FAILED(result))
	{
		return false;
	}

	m_resource = (reinterpret_cast<ID3D11Resource*>(texture));

	return true;
}

void DXTexture::Shutdown()
{
	if(m_texture)
	{
		m_texture->Release();
		m_texture = 0;
	}
}

ID3D11ShaderResourceView* DXTexture::GetTexture()
{
	return m_texture;
}

}