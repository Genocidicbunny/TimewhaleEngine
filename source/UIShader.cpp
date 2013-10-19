#include "UIShader.h"
#include "RenderSystem.h"
#include "TWLogger.h"

namespace Timewhale {

UIShader::UIShader()
{
	m_vertexShader = 0;
	m_pixelShader = 0;
	m_layout = 0;
	m_matrixBuffer = 0;
	m_sampleState = 0;
}


UIShader::UIShader(const UIShader& other)
{
}


UIShader::~UIShader()
{
}

bool UIShader::Initialize()
{
	auto renderer = RenderSystem::get();
	bool result;

    wstring shaderPath = toWideString(renderer->getShaderPath());
    wstring vertexShader = shaderPath + L"\\" + L"RocketVertexShader.hlsl";
    wstring pixelShader = shaderPath + L"\\" + L"RocketPixelShader.hlsl";

	// Initialize the vertex and pixel shaders.
	result = InitializeShader(const_cast<WCHAR*>(vertexShader.c_str()), const_cast<WCHAR*>(pixelShader.c_str()));
	if(!result)
	{
		return false;
	}

	return true;
}

void UIShader::Shutdown()
{
	// Shutdown the vertex and pixel shaders as well as the related objects.
	ShutdownShader();

	return;
}

bool UIShader::Render(int indexCount, XMMATRIX& projectionMatrix, int screenWidth, int screenHeight, int translateX, 
					  int translateY, ID3D11ShaderResourceView* texture)
{
	bool result;

	// Set the shader parameters that it will use for rendering.
	result = SetShaderParameters(projectionMatrix, screenWidth, screenHeight, translateX, translateY, texture);
	if(!result)
	{
		return false;
	}

	// Now render the prepared buffers with the shader.
	RenderShader(indexCount);

	return true;
}

bool UIShader::InitializeShader(WCHAR* vsFilename, WCHAR* psFilename)
{
	HRESULT result;
	ID3D10Blob* errorMessage;
	ID3D10Blob* vertexShaderBuffer;
	ID3D10Blob* pixelShaderBuffer;
	D3D11_INPUT_ELEMENT_DESC polygonLayout[3];
	unsigned int numElements;
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	D3D11_RASTERIZER_DESC scissorEnableDesc;
	D3D11_RASTERIZER_DESC scissorDisableDesc;

	auto renderer = RenderSystem::get();
	HWND hwnd = renderer->m_hwnd;
	ID3D11Device* device = renderer->mRenderer->GetDevice();

	// Initialize the pointers this function will use to null.
	errorMessage = 0;
	vertexShaderBuffer = 0;
	pixelShaderBuffer = 0;

	// Compile the vertex shader code.
	result = D3DCompileFromFile(vsFilename, NULL, NULL, "RocketVertexShader", "vs_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &vertexShaderBuffer, &errorMessage);
	if(FAILED(result))
	{
		// If the shader failed to compile it should have writen something to the error message.
		if(errorMessage)
		{
			renderer->OutputShaderErrorMessage(errorMessage, hwnd, vsFilename);
		}
		// If there was nothing in the error message then it simply could not find the shader file itself.
		else
		{
			log_sxerror("UIShader", "Missing Shader File: %ls", vsFilename);
		}
	}

    // Compile the pixel shader code.
	result = D3DCompileFromFile(psFilename, NULL, NULL, "RocketPixelShader", "ps_4_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, &pixelShaderBuffer, &errorMessage);
    if(FAILED(result))
    {
        // If the shader failed to compile it should have writen something to the error message.
        if(errorMessage)
        {
			renderer->OutputShaderErrorMessage(errorMessage, hwnd, psFilename);
        }
        // If there was  nothing in the error message then it simply could not find the file itself.
        else
        {
			log_sxerror("UIShader", "Missing Shader File: %ls", psFilename);
        }

        return false;
    }

	// Create the vertex shader from the buffer.
	result = device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &m_vertexShader);
	if(FAILED(result))
	{
		log_sxerror("UIShader", "Failed to create the vertex shader.");
		return false;
	}

	// Create the pixel shader from the buffer.
	result = device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &m_pixelShader);
	if(FAILED(result))
	{
		log_sxerror("UIShader", "Failed to create the pixel shader.");
		return false;
	}

	// Now setup the layout of the data that goes into the shader.
	// This setup needs to match the VertexType stucture in the ModelClass and in the shader.
	polygonLayout[0].SemanticName = "POSITION";
	polygonLayout[0].SemanticIndex = 0;
	polygonLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	polygonLayout[0].InputSlot = 0;
	polygonLayout[0].AlignedByteOffset = 0;
	polygonLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[0].InstanceDataStepRate = 0;

	polygonLayout[1].SemanticName = "COLOR";
	polygonLayout[1].SemanticIndex = 0;
	polygonLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	polygonLayout[1].InputSlot = 0;
	polygonLayout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[1].InstanceDataStepRate = 0;

	polygonLayout[2].SemanticName = "TEXCOORD";
	polygonLayout[2].SemanticIndex = 0;
	polygonLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	polygonLayout[2].InputSlot = 0;
	polygonLayout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	polygonLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	polygonLayout[2].InstanceDataStepRate = 0;

	// Get a count of the elements in the layout.
	numElements = sizeof(polygonLayout) / sizeof(polygonLayout[0]);

	// Create the vertex input layout.
	result = device->CreateInputLayout(polygonLayout, numElements, vertexShaderBuffer->GetBufferPointer(), 
					   vertexShaderBuffer->GetBufferSize(), &m_layout);
	if(FAILED(result))
	{
		log_sxerror("UIShader", "Failed to create the vertex input layout.");
		return false;
	}

	// Release the vertex shader buffer and pixel shader buffer since they are no longer needed.
	vertexShaderBuffer->Release();
	vertexShaderBuffer = 0;

	pixelShaderBuffer->Release();
	pixelShaderBuffer = 0;

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(ConstantBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;

	// Create the constant buffer pointer so we can access the vertex shader constant buffer from within this class.
	result = device->CreateBuffer(&matrixBufferDesc, NULL, &m_matrixBuffer);
	if(FAILED(result))
	{
		log_sxerror("UIShader", "Failed to create the constant buffer.");
		return false;
	}

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Create the texture sampler state.
	result = device->CreateSamplerState(&samplerDesc, &m_sampleState);
	if(FAILED(result))
	{
		log_sxerror("UIShader", "Failed to create the texture sampler state.");
		return false;
	}

	// Create the scissor enabled description
	scissorEnableDesc.FillMode = D3D11_FILL_SOLID;
	scissorEnableDesc.CullMode = D3D11_CULL_NONE;
	scissorEnableDesc.ScissorEnable = TRUE;
	scissorEnableDesc.FrontCounterClockwise = TRUE;

	// Create the scissor enabled state
	result = device->CreateRasterizerState(&scissorEnableDesc, &m_ScissorEnable);
	if(FAILED(result))
	{
		log_sxerror("UIShader", "Failed to create the scissor enabled state.");
		return false;
	}

	// Create the scissor disabled description
	scissorDisableDesc.FillMode = D3D11_FILL_SOLID;
	scissorDisableDesc.CullMode = D3D11_CULL_NONE;
	scissorDisableDesc.ScissorEnable = FALSE;
	scissorDisableDesc.FrontCounterClockwise = TRUE;

	result = device->CreateRasterizerState(&scissorDisableDesc, &m_ScissorDisable);
	if(FAILED(result))
	{
		log_sxerror("UIShader", "Failed to create the scissor disabled state.");
		return false;
	}

	return true;
}

void UIShader::ShutdownShader()
{
	// Release the two scissor states
	if(m_ScissorDisable)
	{
		m_ScissorDisable->Release();
		m_ScissorDisable = 0;
	}

	if(m_ScissorEnable)
	{
		m_ScissorEnable->Release();
		m_ScissorEnable = 0;
	}

	// Release the sampler state.
	if(m_sampleState)
	{
		m_sampleState->Release();
		m_sampleState = 0;
	}

	// Release the matrix constant buffer.
	if(m_matrixBuffer)
	{
		m_matrixBuffer->Release();
		m_matrixBuffer = 0;
	}

	// Release the layout.
	if(m_layout)
	{
		m_layout->Release();
		m_layout = 0;
	}

	// Release the pixel shader.
	if(m_pixelShader)
	{
		m_pixelShader->Release();
		m_pixelShader = 0;
	}

	// Release the vertex shader.
	if(m_vertexShader)
	{
		m_vertexShader->Release();
		m_vertexShader = 0;
	}

	return;
}

bool UIShader::SetShaderParameters(XMMATRIX& projectionMatrix, int screenWidth, int screenHeight, int translateX, int translateY, ID3D11ShaderResourceView* texture)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	ConstantBufferType* dataPtr;
	unsigned int bufferNumber;

	auto renderer = RenderSystem::get();
	ID3D11DeviceContext* deviceContext = renderer->mRenderer->GetDeviceContext();

	// Transpose the matrices to prepare them for the shader.
	projectionMatrix = XMMatrixTranspose(projectionMatrix);
	
	// Lock the constant buffer so it can be written to.
	result = deviceContext->Map(m_matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	if(FAILED(result))
	{
		log_sxerror("UIShader", "Failed to lock the constant buffer.");
		return false;
	}

	// Get a pointer to the data in the constant buffer.
	dataPtr = (ConstantBufferType*)mappedResource.pData;

	// Copy the matrices into the constant buffer.
	dataPtr->projection = projectionMatrix;
	dataPtr->screenWH = XMFLOAT2((float)screenWidth, (float)screenHeight);
	dataPtr->translateXY = XMFLOAT2((float)translateX, (float)translateY);

	// Unlock the constant buffer.
	deviceContext->Unmap(m_matrixBuffer, 0);

	// Set the position of the constant buffer in the vertex shader.
	bufferNumber = 0;

	// Finanly set the constant buffer in the vertex shader with the updated values.
	deviceContext->VSSetConstantBuffers(bufferNumber, 1, &m_matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);

	return true;
}

void UIShader::RenderShader(int indexCount)
{
	auto renderer = RenderSystem::get();
	ID3D11DeviceContext* deviceContext = renderer->mRenderer->GetDeviceContext();

	// Set the vertex input layout.
	deviceContext->IASetInputLayout(m_layout);

	// Set the vertex and pixel shaders that will be used to render this triangle.
	deviceContext->VSSetShader(m_vertexShader, NULL, 0);
	deviceContext->PSSetShader(m_pixelShader, NULL, 0);

	// Set the sampler state in the pixel shader.
	deviceContext->PSSetSamplers(0, 1, &m_sampleState);

	// Render the object.
	deviceContext->DrawIndexed(indexCount, 0, 0);


	return;
}

void UIShader::EnableScissor()
{
	auto renderer = RenderSystem::get();

	renderer->mRenderer->GetDeviceContext()->RSSetState(m_ScissorEnable);
}

void UIShader::DisableScissor()
{
	auto renderer = RenderSystem::get();

	renderer->mRenderer->GetDeviceContext()->RSSetState(m_ScissorDisable);
}

}