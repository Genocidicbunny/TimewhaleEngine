#include "Sprite.h"
#include "RenderSystem.h"
#include "TWLogger.h"

namespace Timewhale {

SpriteBatch::SpriteBatch()
{
	m_vertexBuffer = 0;
	m_instanceBuffer = 0;
}


SpriteBatch::SpriteBatch(const SpriteBatch& other)
{
}


SpriteBatch::~SpriteBatch()
{
}

bool SpriteBatch::Initialize()
{
	bool result;

	// Initialize the vertex and index buffer that hold the geometry for the triangle.
	result = InitializeBuffers();
	if(!result)
	{
		return false;
	}

	return true;
}

void SpriteBatch::Shutdown()
{
	// Release the vertex and index buffers.
	ShutdownBuffers();

	return;
}

void SpriteBatch::Render()
{
	// Put the vertex and index buffers on the graphics pipeline to prepare them for drawing.
	RenderBuffers();

	return;
}

int SpriteBatch::GetVertexCount()
{
	return m_vertexCount;
}


int SpriteBatch::GetInstanceCount()
{
	return m_instanceCount;
}


bool SpriteBatch::InitializeBuffers()
{
	auto renderer = RenderSystem::get();

	VertexType* vertices;
	SpriteInstanceData* instances;
	D3D11_BUFFER_DESC vertexBufferDesc, instanceBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, instanceData;
	HRESULT result;

	// Set the number of vertices in the vertex array.
	m_vertexCount = 6;

	// Create the vertex array.
	vertices = new VertexType[m_vertexCount];
	if(!vertices)
	{
		log_sxerror("SpriteBatch", "Failed to allocate space for the vertex array.");
		return false;
	}

	float left = -1.0f, bottom = -1.0f;
	float right = 1.0f, top = 1.0f;

	// Load the vertex array with data.
	// Top Left - 0, Top Right - 1, Bottom Left - 2, Bottom Right - 3
	// First triangle.
	vertices[0].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[1].position = XMFLOAT3(right, bottom, 3.0f);  // Bottom right.
	vertices[2].position = XMFLOAT3(left, bottom, 2.0f);  // Bottom left.

	// Second triangle.
	vertices[3].position = XMFLOAT3(left, top, 0.0f);  // Top left.
	vertices[4].position = XMFLOAT3(right, top, 1.0f);  // Top right.
	vertices[5].position = XMFLOAT3(right, bottom, 3.0f);  // Bottom right.

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
	result = renderer->mRenderer->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &m_vertexBuffer);
	if(FAILED(result))
	{
		log_sxerror("SpriteBatch", "Failed to create the vertex buffer.");
		return false;
	}

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete [] vertices;
	vertices = 0;

	// Set the number of instances in the array.
	m_instanceCount = 2000;

	// Create the instance array.
	instances = new SpriteInstanceData[m_instanceCount];
	if(!instances)
	{
		log_sxerror("SpriteBatch", "Failed to allocate space for the instance array.");
		return false;
	}

	// Initially set the data to zeroes
	memset(instances, 0, (sizeof(SpriteInstanceData) * m_instanceCount));

	// Set up the description of the instance buffer.
	instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	instanceBufferDesc.ByteWidth = sizeof(SpriteInstanceData) * m_instanceCount;
	instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	instanceBufferDesc.MiscFlags = 0;
	instanceBufferDesc.StructureByteStride = 0;

	// Give the subresource structure a pointer to the instance data.
	instanceData.pSysMem = instances;
	instanceData.SysMemPitch = 0;
	instanceData.SysMemSlicePitch = 0;

	// Create the instance buffer.
	result = renderer->mRenderer->GetDevice()->CreateBuffer(&instanceBufferDesc, &instanceData, &m_instanceBuffer);
	if(FAILED(result))
	{
		log_sxerror("SpriteBatch", "Failed to create the instance buffer.");
		return false;
	}

	// Release the instance array now that the instance buffer has been created and loaded.
	delete [] instances;
	instances = 0;

	return true;
}

void SpriteBatch::ShutdownBuffers()
{
	// Release the instance buffer.
	if(m_instanceBuffer)
	{
		m_instanceBuffer->Release();
		m_instanceBuffer = 0;
	}

	// Release the vertex buffer.
	if(m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = 0;
	}

	return;
}

void SpriteBatch::RenderBuffers()
{
	auto renderer = RenderSystem::get();

	unsigned int strides[2];
	unsigned int offsets[2];
	ID3D11Buffer* bufferPointers[2];

	// Set the buffer strides.
	strides[0] = sizeof(VertexType); 
	strides[1] = sizeof(SpriteInstanceData);

	// Set the buffer offsets.
	offsets[0] = 0;
	offsets[1] = 0;

	// Set the array of pointers to the vertex and instance buffers.
	bufferPointers[0] = m_vertexBuffer;	
	bufferPointers[1] = m_instanceBuffer;
    
	// Set the vertex buffer to active in the input assembler so it can be rendered.
	renderer->mRenderer->GetDeviceContext()->IASetVertexBuffers(0, 2, bufferPointers, strides, offsets);

	// Set the type of primitive that should be rendered from this vertex buffer, in this case triangles.
	renderer->mRenderer->GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return;
}

bool SpriteBatch::UpdateBuffers(std::vector<SpriteInstanceData> data)
{
	auto renderer = RenderSystem::get();

	HRESULT result;
	SpriteInstanceData* instances;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	// Create the instance array.
	instances = new SpriteInstanceData[m_instanceCount];
	if(!instances)
	{
		log_sxerror("SpriteBatch", "Failed to allocate space for the instance array.");
		return false;
	}

	// Initially set the data to zeroes
	memset(instances, 0, (sizeof(SpriteInstanceData) * m_instanceCount));

	// Loop through the data and set up each instance
	for(size_t i = 0; i < m_instanceCount && i < data.size(); i++) {
		instances[i].color = data[i].color;
		instances[i].uv = data[i].uv;
		instances[i].xywh = data[i].xywh;
	}

	// Lock the buffer so that it can be written to
	result = renderer->mRenderer->GetDeviceContext()->Map(m_instanceBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		log_sxerror("SpriteBatch", "Failed to lock the instance buffer.");
		return false;
	}

	// Get a pointer to the data in the buffer
	SpriteInstanceData* dataPtr = (SpriteInstanceData*)mappedResource.pData;

	//Copy the data into the buffer
	memcpy(dataPtr, (void*)instances, (sizeof(SpriteInstanceData) * m_instanceCount));

	// Unlock the buffer
	renderer->mRenderer->GetDeviceContext()->Unmap(m_instanceBuffer, 0);

	// Release the temporary array
    if (instances)
	    delete [] instances;
	instances = nullptr;

	return true;
	
}

}