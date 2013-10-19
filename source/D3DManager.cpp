#include "D3DManager.h"
#include "TWLogger.h"

namespace Timewhale {
D3DManager::D3DManager(void)
{
    m_swapChain = NULL;
    m_device = NULL;
    m_deviceContext = NULL;
    m_renderTargetView = NULL;
    m_depthStencilBuffer = NULL;
    m_depthStencilState = NULL;
    m_depthStencilView = NULL;
    m_rasterState = NULL;
    m_depthDisabledStencilState = 0;
    m_alphaEnableBlendingState = 0;
    m_alphaDisableBlendingState = 0;
	m_videoCardMemory = 0;
}


D3DManager::~D3DManager(void)
{
}

bool D3DManager::Initialize(int screenWidth, int screenHeight, bool vsync, HWND hwnd, bool fullscreen, 
                          float screenDepth, float screenNear) {
    HRESULT result;
    IDXGIFactory* factory;
    IDXGIAdapter* adapter;
    IDXGIOutput* adapterOutput;
    unsigned int numModes, i, numerator, denominator, stringLength;
    DXGI_MODE_DESC* displayModeList;
    DXGI_ADAPTER_DESC adapterDesc;
    int error;
    DXGI_SWAP_CHAIN_DESC swapChainDesc;
    D3D_FEATURE_LEVEL featureLevel;
    ID3D11Texture2D* backBufferPtr;
    D3D11_TEXTURE2D_DESC depthBufferDesc;
    D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
    D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
    D3D11_RASTERIZER_DESC rasterDesc;
    D3D11_VIEWPORT viewport;
    float fieldOfView, screenAspect;
    D3D11_DEPTH_STENCIL_DESC depthDisabledStencilDesc;
    D3D11_BLEND_DESC blendStateDescription;
	std::vector<IDXGIAdapter*> vAdapters;
	int largestAdapter = 0;
	int videoCardMemory;

    // Store our screen width and height
    m_ScreenWidth = screenWidth;
    m_ScreenHeight = screenHeight;

    // Store the vsync setting.
    m_vsync_enabled = vsync;

    // Create a DirectX graphics interface factory.
    result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create a DXGI factory.");
        return false;
    }

	// Loop through each adapter on the computer
	for(UINT i = 0; factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i)
	{
		// Store the adapter
		vAdapters.push_back(adapter);

		// Get the adapter (video card) description.
		result = adapter->GetDesc(&adapterDesc);
		if(FAILED(result))
		{
			log_sxerror("D3DManager", "Failed to get the video card description.");
			return false;
		}

		// Store the dedicated video card memory in megabytes.
		videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

		// If the memory is larger than our current memory, save this index
		if(videoCardMemory > m_videoCardMemory) {
			m_videoCardMemory = videoCardMemory;
			largestAdapter = i;

			// Convert the name of the video card to a character array and store it.
			error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
			if(error != 0)
			{
				log_sxerror("D3DManager", "Failed to get the name of the graphics card.");
				return false;
			}
		}
	}

    // Enumerate the primary adapter output (monitor).
	result = vAdapters.at(0)->EnumOutputs(0, &adapterOutput);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to grab the monitor.");
        return false;
    }

    // Get the number of modes that fit the DXGI_FORMAT_R8G8B8A8_UNORM display format for the adapter output (monitor).
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to get a list of display modes.");
        return false;
    }

    // Create a list to hold all the possible display modes for this monitor/video card combination.
    displayModeList = new DXGI_MODE_DESC[numModes];
    if(!displayModeList)
    {
		log_sxerror("D3DManager", "Failed to create a display mode list for the monitor/gfx card combo.");
        return false;
    }

    // Now fill the display mode list structures.
    result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to fill the display mode list structures.");
        return false;
    }

    // Now go through all the display modes and find the one that matches the screen width and height.
    // When a match is found store the numerator and denominator of the refresh rate for that monitor.
    for(i=0; i<numModes; i++)
    {
            if(displayModeList[i].Width == (unsigned int)screenWidth)
            {
                    if(displayModeList[i].Height == (unsigned int)screenHeight)
                    {
                            numerator = displayModeList[i].RefreshRate.Numerator;
                            denominator = displayModeList[i].RefreshRate.Denominator;
                    }
            }
    }

    // Release the display mode list.
    delete [] displayModeList;
    displayModeList = 0;

    // Release the adapter output.
    adapterOutput->Release();
    adapterOutput = 0;

    // Release the factory.
    factory->Release();
    factory = 0;

    // Initialize the swap chain description.
    ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

    // Set to a single back buffer.
    swapChainDesc.BufferCount = 1;

    // Set the width and height of the back buffer.
    swapChainDesc.BufferDesc.Width = screenWidth;
    swapChainDesc.BufferDesc.Height = screenHeight;

    // Set regular 32-bit surface for the back buffer.
    swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

    // Set the refresh rate of the back buffer.
    if(m_vsync_enabled)
    {
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;//numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;//denominator;
    }
    else
    {
        swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
    }

    // Set the usage of the back buffer.
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    // Set the handle for the window to render to.
    swapChainDesc.OutputWindow = hwnd;

    // Turn multisampling off.
    swapChainDesc.SampleDesc.Count = 1;
    swapChainDesc.SampleDesc.Quality = 0;

    // Set to full screen or windowed mode.
    if(fullscreen)
    {
        swapChainDesc.Windowed = false;
    }
    else
    {
        swapChainDesc.Windowed = true;
    }

    // Set the scan line ordering and scaling to unspecified.
    swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
    swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

    // Discard the back buffer contents after presenting.
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	//swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

    // Don't set the advanced flags.
    swapChainDesc.Flags = 0;

    // Grab the highest feature level
	result = D3D11CreateDevice(vAdapters.at(largestAdapter), D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, NULL, 0,
                       D3D11_SDK_VERSION, NULL, &featureLevel, NULL );
	if(FAILED(result))
	{
		log_sxerror("D3DManager", "Failed to retrieve the featurelevel.");
		ErrorDescription(result);

		// Try to use the default adapter
		largestAdapter = 0;

		result = D3D11CreateDevice(vAdapters.at(largestAdapter), D3D_DRIVER_TYPE_UNKNOWN, NULL, 0, NULL, 0,
                       D3D11_SDK_VERSION, NULL, &featureLevel, NULL );
		if(FAILED(result))
		{
			log_sxerror("D3DManager", "Failed to retrieve the featurelevel the second time.");
			ErrorDescription(result);
			return false;
		}
	}

    
#ifdef _DEBUG
	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(vAdapters.at(largestAdapter), D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT , &featureLevel, 1, 
                                            D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the Direct3D device, context, and swap chain.");
        return false;
    }

	result = m_device->QueryInterface((IID)IID_ID3D11Debug, (void**)&m_Debug);
	if(FAILED(result))
	{
		log_sxerror("D3DManager", "Failed to create a debugging interface for the device. Debugging may not work");
	} else {
		result = m_Debug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		if(FAILED(result)) {
			log_sxerror("D3DManager", "Failed to enable detailed object reporting.");
		}	
	}

#else
	// Create the swap chain, Direct3D device, and Direct3D device context.
	result = D3D11CreateDeviceAndSwapChain(vAdapters.at(largestAdapter), D3D_DRIVER_TYPE_UNKNOWN, NULL, D3D11_CREATE_DEVICE_BGRA_SUPPORT , &featureLevel, 1, 
                                            D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the Direct3D device, context, and swap chain.");
        return false;
    }

#endif

	// Release each adapter.
	for(auto it = vAdapters.begin(); it != vAdapters.end(); ++it)
	{
		(*it)->Release();
		(*it) = 0;
	}

    // Get the pointer to the back buffer.
    result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to get a pointer to the back buffer.");
        return false;
    }

    // Create the render target view with the back buffer pointer.
    result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the render target view.");
        return false;
    }

    // Release pointer to the back buffer as we no longer need it.
    backBufferPtr->Release();
    backBufferPtr = 0;

    // Initialize the description of the depth buffer.
    ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

    // Set up the description of the depth buffer.
    depthBufferDesc.Width = screenWidth;
    depthBufferDesc.Height = screenHeight;
    depthBufferDesc.MipLevels = 1;
    depthBufferDesc.ArraySize = 1;
    depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthBufferDesc.SampleDesc.Count = 1;
    depthBufferDesc.SampleDesc.Quality = 0;
    depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
    depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthBufferDesc.CPUAccessFlags = 0;
    depthBufferDesc.MiscFlags = 0;

    // Create the texture for the depth buffer using the filled out description.
    result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the depth buffer.");
        return false;
    }

    // Initialize the description of the stencil state.
    ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

    // Set up the description of the stencil state.
    depthStencilDesc.DepthEnable = true;
    depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

    depthStencilDesc.StencilEnable = true;
    depthStencilDesc.StencilReadMask = 0xFF;
    depthStencilDesc.StencilWriteMask = 0xFF;

    // Stencil operations if pixel is front-facing.
    depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Stencil operations if pixel is back-facing.
    depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the depth stencil state.
    result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the depth stencil state.");
        return false;
    }

    // Set the depth stencil state.
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

    // Initailze the depth stencil view.
    ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

    // Set up the depth stencil view description.
    depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    depthStencilViewDesc.Texture2D.MipSlice = 0;

    // Create the depth stencil view.
    result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the depth stencil view.");
        return false;
    }

    // Bind the render target view and depth stencil buffer to the output render pipeline.
    m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);

    // Setup the raster description which will determine how and what polygons will be drawn.
    rasterDesc.AntialiasedLineEnable = false;
    rasterDesc.CullMode = D3D11_CULL_BACK;
    rasterDesc.DepthBias = 0;
    rasterDesc.DepthBiasClamp = 0.0f;
    rasterDesc.DepthClipEnable = true;
    rasterDesc.FillMode = D3D11_FILL_SOLID;
    rasterDesc.FrontCounterClockwise = false;
    rasterDesc.MultisampleEnable = false;
    rasterDesc.ScissorEnable = false;
    rasterDesc.SlopeScaledDepthBias = 0.0f;

    // Create the rasterizer state from the description we just filled out.
    result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the rasterizer state.");
        return false;
    }

    // Now set the rasterizer state.
    m_deviceContext->RSSetState(m_rasterState);

    // Setup the viewport for rendering.
    viewport.Width = (float)screenWidth;
    viewport.Height = (float)screenHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    viewport.TopLeftX = 0.0f;
    viewport.TopLeftY = 0.0f;

    // Create the viewport.
    m_deviceContext->RSSetViewports(1, &viewport);

    // Setup the projection matrix.
	fieldOfView = (float)XM_PI / 4.0f;
	assert(screenWidth != 0);
	assert(screenHeight != 0);
    screenAspect = (float)screenWidth / (float)screenHeight;

    // Create the projection matrix for 3D rendering.
	XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth));

    // Initialize the world and view matrix to the identity matrix.
    XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());

    // Create an orthographic projection matrix for 2D rendering.
    XMStoreFloat4x4(&m_orthoMatrix, XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth));

    // Clear the second depth stencil state before setting the parameters.
    ZeroMemory(&depthDisabledStencilDesc, sizeof(depthDisabledStencilDesc));

    // Now create a second depth stencil state which turns off the Z buffer for 2D rendering.  The only difference is 
    // that DepthEnable is set to false, all other parameters are the same as the other depth stencil state.
    depthDisabledStencilDesc.DepthEnable = false;
    depthDisabledStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    depthDisabledStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
    depthDisabledStencilDesc.StencilEnable = true;
    depthDisabledStencilDesc.StencilReadMask = 0xFF;
    depthDisabledStencilDesc.StencilWriteMask = 0xFF;
    depthDisabledStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
    depthDisabledStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
    depthDisabledStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
    depthDisabledStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
    depthDisabledStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

    // Create the state using the device.
    result = m_device->CreateDepthStencilState(&depthDisabledStencilDesc, &m_depthDisabledStencilState);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the depth disabled stencil state.");
        return false;
    }

    // Clear the blend state description.
    ZeroMemory(&blendStateDescription, sizeof(D3D11_BLEND_DESC));

    // Create an alpha enabled blend state description.

    blendStateDescription.RenderTarget[0].BlendEnable = TRUE;
    blendStateDescription.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
    blendStateDescription.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	blendStateDescription.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
    blendStateDescription.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
    blendStateDescription.RenderTarget[0].RenderTargetWriteMask = 0x0f;

	//blendStateDescription.RenderTarget[0].BlendEnable            = true;
    //blendStateDescription.RenderTarget[0].SrcBlend               = D3D11_BLEND_SRC_COLOR; // This one fucks by adding white
    //blendStateDescription.RenderTarget[0].DestBlend              = D3D11_BLEND_DEST_COLOR; // This one produces fail whale, dusky background, lite trees
    //blendStateDescription.RenderTarget[0].BlendOp                = D3D11_BLEND_OP_ADD;
    //blendStateDescription.RenderTarget[0].SrcBlendAlpha          = D3D11_BLEND_SRC_ALPHA;
    //blendStateDescription.RenderTarget[0].DestBlendAlpha         = D3D11_BLEND_DEST_ALPHA;
    //blendStateDescription.RenderTarget[0].BlendOpAlpha           = D3D11_BLEND_OP_ADD;
    //blendStateDescription.RenderTarget[0].RenderTargetWriteMask  = 7;

    // Create the blend state using the description.
    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaEnableBlendingState);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the alpha blending state.");
        return false;
    }

    // Modify the description to create an alpha disabled blend state description.
    blendStateDescription.RenderTarget[0].BlendEnable = FALSE;

    // Create the blend state using the description.
    result = m_device->CreateBlendState(&blendStateDescription, &m_alphaDisableBlendingState);
    if(FAILED(result))
    {
		log_sxerror("D3DManager", "Failed to create the alpha disabled blending state.");
        return false;
    }

    return true;
}

bool D3DManager::Resize(int screenWidth, int screenHeight, float screenDepth, float screenNear)
{
	if(m_swapChain) {
		m_deviceContext->OMSetRenderTargets(0, 0, 0);

        // Release all outstanding references to the swap chain's buffers.
        m_renderTargetView->Release();

        HRESULT result;
        // Preserve the existing buffer count and format.
        // Automatically choose the width and height to match the client rect for HWNDs.
        result = m_swapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);                                        
        if(FAILED(result))
		{
			log_sxerror("D3DManager", "Failed to resize our buffers.");
			return false;
		}

        // Get buffer and create a render-target-view.
        ID3D11Texture2D* pBuffer;
        result = m_swapChain->GetBuffer(0, __uuidof( ID3D11Texture2D),
                                        (void**) &pBuffer );
        if(FAILED(result))
		{
			log_sxerror("D3DManager", "Failed to get a pointer to the back buffer.");
			return false;
		}

        result = m_device->CreateRenderTargetView(pBuffer, NULL,
                                                    &m_renderTargetView);
        if(FAILED(result))
		{
			log_sxerror("D3DManager", "Failed to recreate the render target view.");
			return false;
		}

        pBuffer->Release();

        m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, NULL );

		// Setup the viewport for rendering.
		D3D11_VIEWPORT viewport;
        viewport.Width = (float)screenWidth;
        viewport.Height = (float)screenHeight;
        viewport.MinDepth = 0.0f;
        viewport.MaxDepth = 1.0f;
        viewport.TopLeftX = 0.0f;
        viewport.TopLeftY = 0.0f;

        // Create the viewport.
        m_deviceContext->RSSetViewports(1, &viewport);

		 // Setup the projection matrix.
		float fieldOfView = (float)XM_PI / 4.0f;
		assert(screenWidth != 0);
		assert(screenHeight != 0);
        float screenAspect = (float)screenWidth / (float)screenHeight;

        // Create the projection matrix for 3D rendering.
        XMStoreFloat4x4(&m_projectionMatrix, XMMatrixPerspectiveFovLH(fieldOfView, screenAspect, screenNear, screenDepth));

        // Initialize the world and view matrix to the identity matrix.
        XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_viewMatrix, XMMatrixIdentity());

        // Create an orthographic projection matrix for 2D rendering.
        XMStoreFloat4x4(&m_orthoMatrix, XMMatrixOrthographicLH((float)screenWidth, (float)screenHeight, screenNear, screenDepth));

		// Store our screen dimensions
		m_ScreenWidth = screenWidth;
		m_ScreenHeight = screenHeight;
	}

	return true;
}

void D3DManager::Shutdown()
{
    // Before shutting down set to windowed mode or when you release the swap chain it will throw an exception.
    if(m_swapChain)
    {
        m_swapChain->SetFullscreenState(false, NULL);
    }

    if(m_alphaEnableBlendingState)
    {
        m_alphaEnableBlendingState->Release();
        m_alphaEnableBlendingState = 0;
    }

    if(m_alphaDisableBlendingState)
    {
        m_alphaDisableBlendingState->Release();
        m_alphaDisableBlendingState = 0;
    }

    if(m_depthDisabledStencilState)
    {
        m_depthDisabledStencilState->Release();
        m_depthDisabledStencilState = 0;
    }

    if(m_rasterState)
    {
        m_rasterState->Release();
        m_rasterState = 0;
    }

    if(m_depthStencilView)
    {
        m_depthStencilView->Release();
        m_depthStencilView = 0;
    }

    if(m_depthStencilState)
    {
        m_depthStencilState->Release();
        m_depthStencilState = 0;
    }

    if(m_depthStencilBuffer)
    {
        m_depthStencilBuffer->Release();
        m_depthStencilBuffer = 0;
    }

    if(m_renderTargetView)
    {
		m_renderTargetView->Release();
		m_renderTargetView = 0;
    }

    if(m_deviceContext)
    {
        m_deviceContext->Release();
        m_deviceContext = 0;
    }

    if(m_device)
    {
        m_device->Release();
        m_device = 0;
    }

    if(m_swapChain)
    {
        m_swapChain->Release();
        m_swapChain = 0;
    }

    return;
}

void D3DManager::BeginScene(float red, float green, float blue, float alpha)
{
    float color[4];

    // Setup the color to clear the buffer to.
    color[0] = red;
    color[1] = green;
    color[2] = blue;
    color[3] = alpha;

    // Clear the back buffer.
    m_deviceContext->ClearRenderTargetView(m_renderTargetView, color);

    // Clear the depth buffer.
    m_deviceContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

    return;
}

void D3DManager::EndScene()
{
    // Present the back buffer to the screen since rendering is complete.
    if(m_vsync_enabled)
    {
        // Lock to screen refresh rate.
        m_swapChain->Present(1, 0);
    }
    else
    {
        // Present as fast as possible.
        m_swapChain->Present(0, 0);
    }

    return;
}

ID3D11Device* D3DManager::GetDevice()
{
    return m_device;
}


ID3D11DeviceContext* D3DManager::GetDeviceContext()
{
    return m_deviceContext;
}

void D3DManager::GetProjectionMatrix(XMMATRIX& projectionMatrix)
{
    projectionMatrix = XMLoadFloat4x4(&m_projectionMatrix);
    return;
}


void D3DManager::GetWorldMatrix(XMMATRIX& worldMatrix)
{
    worldMatrix = XMLoadFloat4x4(&m_worldMatrix);
    return;
}


void D3DManager::GetOrthoMatrix(XMMATRIX& orthoMatrix)
{
    orthoMatrix = XMLoadFloat4x4(&m_orthoMatrix);
    return;
}

void D3DManager::GetViewMatrix(XMMATRIX& viewMatrix)
{
	viewMatrix = XMLoadFloat4x4(&m_viewMatrix);
	return;
}

void D3DManager::GetVideoCardInfo(char* cardName, int& memory)
{
    strcpy_s(cardName, 128, m_videoCardDescription);
    memory = m_videoCardMemory;
    return;
}

int D3DManager::GetScreenWidth() 
{
    return m_ScreenWidth;
}

int D3DManager::GetScreenHeight() 
{
    return m_ScreenHeight;
}

void D3DManager::TurnZBufferOn()
{
    m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);
    return;
}


void D3DManager::TurnZBufferOff()
{
    m_deviceContext->OMSetDepthStencilState(m_depthDisabledStencilState, 1);
    return;
}

void D3DManager::TurnOnAlphaBlending()
{
    float blendFactor[4];

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;
        
    // Turn on the alpha blending.
    m_deviceContext->OMSetBlendState(m_alphaEnableBlendingState, blendFactor, 0xffffffff);

    return;
}

void D3DManager::TurnOffAlphaBlending()
{
    float blendFactor[4];   

    // Setup the blend factor.
    blendFactor[0] = 0.0f;
    blendFactor[1] = 0.0f;
    blendFactor[2] = 0.0f;
    blendFactor[3] = 0.0f;
        
    // Turn off the alpha blending.
    m_deviceContext->OMSetBlendState(m_alphaDisableBlendingState, blendFactor, 0xffffffff);

    return;
}

void D3DManager::ErrorDescription(HRESULT hr) 
{
	switch(hr) {
		case D3D11_ERROR_FILE_NOT_FOUND:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"The file was not found.");
			break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"There are too many unique instances of a particular type of state object.");
			break;
		case D3D11_ERROR_TOO_MANY_UNIQUE_VIEW_OBJECTS:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"There are too many unique instances of a particular type of view object.");
			break;
		case D3D11_ERROR_DEFERRED_CONTEXT_MAP_WITHOUT_INITIAL_DISCARD:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"The first call to ID3D11DeviceContext::Map after either ID3D11Device::CreateDeferredContext or ID3D11DeviceContext::FinishCommandList per Resource was not D3D11_MAP_WRITE_DISCARD.");
			break;
		case E_FAIL:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"Attempted to create a device with the debug layer enabled and the layer is not installed.");
			break;
		case E_INVALIDARG:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"An invalid parameter was passed to the returning function.");
			break;
		case E_OUTOFMEMORY:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"Direct3D could not allocate sufficient memory to complete the call.");
			break;
		case S_FALSE:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"Alternate success value, indicating a successful but nonstandard completion (the precise meaning depends on context).");
			break;
		case S_OK:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"No error occurred.");
			break;
		default:
			log_sxerror("D3DManager", "DirectX Error: %s", 
				"Some really weird shit just happened.");
	}
}

}