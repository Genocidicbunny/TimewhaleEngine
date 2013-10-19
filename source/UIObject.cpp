#include "UIObject.h"
#include "RenderSystem.h"
#include "TWLogger.h"

namespace Timewhale {

UIObject::UIObject()
{
	m_vertexBuffer = 0;
	m_indexBuffer = 0;
}


UIObject::UIObject(const UIObject& other)
{
}


UIObject::~UIObject()
{
}

bool UIObject::Initialize(Rocket::Core::Vertex* vertices, int num_vertices, int* indices, int num_indices, uint16_t textureID, TextureDetails tex)
{
	bool result;

	// Store the texture ID
	m_textureID = textureID;

	// Initialize the vertex and index buffer that hold the geometry for UI element.
	result = InitializeBuffers(vertices, num_vertices, indices, num_indices, tex);
	if(!result)
	{
		return false;
	}

	return true;
}

void UIObject::Shutdown()
{
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void UIObject::Render()
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers();

	return;
}

int UIObject::GetIndexCount()
{
	return m_indexCount;
}

uint16_t UIObject::GetTextureID()
{
	return m_textureID;
}

bool UIObject::InitializeBuffers(Rocket::Core::Vertex* rocketVertices, int num_vertices, int* rocketIndices, int num_indices, TextureDetails tex)
{
	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;
	HRESULT result;

	auto renderer = RenderSystem::get();
	ID3D11Device* device = renderer->mRenderer->GetDevice();

	// Set the number of vertices in the vertex array.
	m_vertexCount = num_vertices;

	// Set the number of indices in the index array.
	m_indexCount = num_indices;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		log_sxerror("UIObject", "Failed to allocate space for the vertex array.");
		return false;
	}

	// Create the index array.
	indices = new unsigned long[m_indexCount];
	if(!indices)
	{
		log_sxerror("UIObject", "Failed to allocate space for the index array.");
		return false;
	}

	// Load the vertex array with the rocket data
	XMFLOAT2 uvdata;
	for(int i = 0; i < num_vertices; i++) {
		vertices[i].position = XMFLOAT3(rocketVertices[i].position.x, -rocketVertices[i].position.y, 0.0f);
		vertices[i].color = XMFLOAT4(rocketVertices[i].colour.red / 255.0f, rocketVertices[i].colour.green / 255.0f, rocketVertices[i].colour.blue / 255.0f, rocketVertices[i].colour.alpha / 255.0f);
		
		uvdata = XMFLOAT2(rocketVertices[i].tex_coord[0], rocketVertices[i].tex_coord[1]);

		// Recalculate uv data relative to the pack
		uvdata.x = tex._u + uvdata.x * tex._uvwidth;
		uvdata.y = tex._v + uvdata.y * tex._uvheight;

		vertices[i].texture = uvdata;
	}

	// Load the index array with rocket data
	for(int i = 0; i < num_indices; i++) {
		indices[i] = rocketIndices[i];
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * m_vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;

	// Now create the vertex buffer.
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		log_sxerror("UIObject", "Failed to create the vertex buffer.");
		return false;
	}

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;

	// Create the index buffer.
	result = device->CreateBuffer(&indexBufferDesc, &indexData, &m_indexBuffer);
	if(FAILED(result))
	{
		log_sxerror("UIObject", "Failed to create the index buffer.");
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	delete [] indices;
	indices = 0;

	return true;
}

void UIObject::ShutdownBuffers()
{
	// Release the index buffer.
	if(m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void UIObject::RenderBuffers()
{
	unsigned int stride;
	unsigned int offset;

	auto renderer = RenderSystem::get();
	ID3D11DeviceContext* deviceContext = renderer->mRenderer->GetDeviceContext();

	// Set vertex buffer stride and offset.
	stride = sizeof(VertexType); 
	offset = 0;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

}