#include "RenderSystem.h"
#include "UIManager.h"
#include "TWLogger.h"

namespace Timewhale {

    string shaderPath = "Assets\\shaders";

    RenderSystemPtr RenderSystem::sSystem = nullptr;

    RenderSystemPtr const RenderSystem::create() {
        if (!sSystem) {
            sSystem = RenderSystemPtr(new RenderSystem());
        }
        return sSystem;
    }

RenderSystem::RenderSystem(void)
{
	mRenderer = NULL;
	m_SpriteBatch = 0;
	m_SpriteShader = 0;

	currentPackID = currentTextureID = 1;
}


RenderSystem::~RenderSystem(void)
{
}

bool RenderSystem::Initialize(int screenWidth, int screenHeight, HWND hwnd) {
    bool result;

	mRenderer = new Renderer();
	if(!mRenderer) return false;

	// Store the window handle
	m_hwnd = hwnd;

    // Initialize the DXmanager
    result = mRenderer->Initialize(screenWidth, screenHeight, VSYNC_ENABLED, hwnd, FULL_SCREEN, SCREEN_DEPTH, SCREEN_NEAR);
    if(!result)
    {
		log_sxerror("RenderSystem", "Failed to initialize the D3DManager.");
        return false;
    }
	
	// Create the sprite batch.
	m_SpriteBatch = new SpriteBatch;
	if(!m_SpriteBatch)
	{
		return false;
	}

	// Initialize the sprite batch.
	result = m_SpriteBatch->Initialize();
	if(!result)
	{
		log_sxerror("RenderSystem", "Failed to initialize the SpriteBatch.");
		return false;
	}

	// Create the sprite shader object.
	m_SpriteShader = new SpriteShader;
	if(!m_SpriteShader)
	{
		return false;
	}

	// Initialize the sprite shader object.
	result = m_SpriteShader->Initialize();
	if(!result)
	{
		log_sxerror("RenderSystem", "Failed to initialize the SpriteShader.");
		return false;
	}

	// Create a camera
	m_Camera = new Camera;

	// Turn Z Buffer off for 2D
	mRenderer->TurnZBufferOff();

	// Turn on Alpha Blending
	mRenderer->TurnOnAlphaBlending();

	log_sxinfo("RenderSystem", "Renderer initialized properly.");

    return true;
}

bool RenderSystem::Resize(int windowWidth, int windowHeight)
{
	return mRenderer->Resize(windowWidth, windowHeight, SCREEN_DEPTH, SCREEN_NEAR);
}

void RenderSystem::Shutdown() {
	// Loop through the packs and shut them down
	std::map<uint16_t, DXTexture*>::iterator it;
	for(it = m_Packs.begin(); it != m_Packs.end(); it++) {
		it->second->Shutdown();
		delete it->second;
		it->second = 0;
	}

	// Release the sprite shader object.
	if(m_SpriteShader)
	{
		m_SpriteShader->Shutdown();
		delete m_SpriteShader;
		m_SpriteShader = 0;
	}

	// Release the sprite batch.
	if(m_SpriteBatch)
	{
		m_SpriteBatch->Shutdown();
		delete m_SpriteBatch;
		m_SpriteBatch = 0;
	}

	if(mRenderer)
    {
        mRenderer->Shutdown();
        delete mRenderer;
        mRenderer = NULL;
    }
}

bool RenderSystem::Draw() {
    bool result;

    // Render the graphics scene.
    result = Render();
    if(!result)
    {
            return false;
    }

    return true;
}

bool RenderSystem::Render() {
	XMMATRIX orthoMatrix;
	bool result = true;

	// Clear the buffers to begin the scene.
	mRenderer->BeginScene(0.0f, 0.0f, 0.0f, 1.0f);

	// Get the world, view, and projection matrices from the camera and d3d objects.
	mRenderer->GetOrthoMatrix(orthoMatrix);

    for (auto spriteVector : m_Sprites) {
        // Copy these sprites instance data onto the GPU
        m_SpriteBatch->UpdateBuffers(spriteVector.second);
        // Put the sprite vertex and instance buffers on the graphics pipeline to prepare them for drawing.
        m_SpriteBatch->Render();

		// Find the pack that we need to render with
		auto it = m_Packs.find(spriteVector.first);

        // Render the sprites using the shader.
		result = m_SpriteShader->Render(
            m_SpriteBatch->GetVertexCount(), 
			spriteVector.second.size(), 
            orthoMatrix, 
			mRenderer->GetScreenWidth(), 
            mRenderer->GetScreenHeight(), 
            m_Camera->GetX(), 
            m_Camera->GetY(), 
            it->second->GetTexture(),
			m_Camera->GetScale());
		if(!result)
		{
			log_sxerror("RenderSystem", "Did not render the last frame properly.");
			return false;
		}
    }

	// Render the UI
	UIManager::get()->render();

    // Present the rendered scene to the screen.
	mRenderer->EndScene();

    return result;
}

void RenderSystem::BufferScenes(const SceneRenderData &data, const size_t number) {
	//std::vector<SceneRenderData> scenes(data);
	// Stores the uv data for the particular sprite
	XMFLOAT4 uvdata;
    
    m_Sprites.clear();
    // If there is a scene data, and the render buffer has sprites in it.
   // if (data.size() > 0 && data[0].rbuf_vec.size() > 0) {
	if(number > 0 ) {
        // Get the camera position from the scene data.
		m_Camera->SetPosition(data.SceneCamera->GetX(), data.SceneCamera->GetY());
		m_Camera->SetScale(data.SceneCamera->GetScale());

		// Get the packID of the first sprite
		uint16_t texID = data.rbuf_vec[0].textureID;
		auto texit = m_Textures.find(texID);
		auto packit = m_Packs.find(texit->second);
		uint16_t packID = packit->first;

		// Create a new sprite layer vector.
        m_Sprites.push_back(make_pair(packID, vector<SpriteInstanceData>()));

		int index = 0;
		//for(auto sprite : data.rbuf_vec) 
		for(size_t i = 0; i < number; ++i)
		{
			auto& sprite = data.rbuf_vec[i];
			// Grab this sprites pack id
			auto texit = m_Textures.find(sprite.textureID);
			auto packit = m_Packs.find(texit->second);
			uint16_t currpack = packit->first;

			// If we changed packs, create a new vector
			if(currpack != packID) 
			{
				index = m_Sprites.size();
				packID = currpack;
				m_Sprites.push_back(make_pair(packID, vector<SpriteInstanceData>()));
			}

			assert(sprite->numHorizontalSpriteFrames != 0);
			assert(sprite->numVerticalSpriteFrames != 0);
			// Create the uvdata for this sprite
            uvdata = XMFLOAT4(
				(float)(sprite.currentSpriteFrame % sprite.numHorizontalSpriteFrames) / (float)sprite.numHorizontalSpriteFrames,
				(float)(sprite.currentSpriteFrame / sprite.numHorizontalSpriteFrames) / (float)sprite.numVerticalSpriteFrames,
				(float)((sprite.currentSpriteFrame % sprite.numHorizontalSpriteFrames) + 1) / (float)sprite.numHorizontalSpriteFrames,
				(float)((sprite.currentSpriteFrame / sprite.numHorizontalSpriteFrames) + 1) / (float)sprite.numVerticalSpriteFrames);

			// Grab the texinfo for this sprite
			auto it = m_TexInfo.find(sprite.textureID);

			// Recalculate uv data relative to the pack
			uvdata.x = it->second._u + uvdata.x * it->second._uvwidth;
			uvdata.y = it->second._v + uvdata.y * it->second._uvheight;
			uvdata.z = it->second._u + uvdata.z * it->second._uvwidth;
			uvdata.w = it->second._v + uvdata.w * it->second._uvheight;

            m_Sprites[index].second.push_back(
				SpriteInstanceData(
					XMFLOAT3(sprite.r, sprite.g, sprite.b),
                    XMFLOAT4(sprite.x, sprite.y, sprite.width, sprite.height),
					uvdata));
        }
    }
}

PackDetails RenderSystem::CreatePack(uint8_t*data, uint32_t len) 
{
	bool result;
	uint32_t width, height;

	// Create a new DXTexture
	DXTexture* temp = new DXTexture;
	if(!temp)
	{
		log_sxerror("RenderSystem", "Ran out of memory to allocate for the texture pack.");
		return PackDetails(0, 0, 0);
	}

	// Initialize the texture
	result = temp->Initialize(data, len, width, height);
	if(!result)
	{
		log_sxerror("RenderSystem", "Failed to load your texture pack properly.");
		return PackDetails(0, 0, 0);
	}

	// Create a new pack info
	PackDetails pack(currentPackID++, width, height);

	// Insert the pack into the map
	m_Packs.insert(std::pair<uint16_t, DXTexture*>(pack._pack_id, temp));

	// Insert the texture info into the other map
	m_PackInfo.insert(std::pair<uint16_t, PackDetails>(pack._pack_id, pack));

	return pack;
}

uint16_t RenderSystem::CreatePack(ID3D11Texture2D* texture, uint32_t width, uint32_t height)
{
	bool result;

	// Create a new DXTexutre
	DXTexture* temp = new DXTexture;
	if(!temp)
	{
		log_sxerror("RenderSystem", "Ran out of memory to allocate for the texture pack.");
		return 0;
	}

	// Initialize the texture
	result = temp->Initialize(texture);
	if(!result)
	{
		log_sxerror("RenderSystem", "Failed to load your texture pack properly.");
		return 0;
	}

	// Create a new pack info
	PackDetails pack(currentPackID++, width, height);

	// Insert the pack into the map
	m_Packs.insert(std::pair<uint16_t, DXTexture*>(pack._pack_id, temp));

	// Insert the texture info into the other map
	m_PackInfo.insert(std::pair<uint16_t, PackDetails>(pack._pack_id, pack));

	return (currentPackID - 1);
}

TextureDetails RenderSystem::CreateTextureDetails(TextureDetails texinfo, uint16_t packID)
{
	// Create a new texture details
	TextureDetails tex(currentTextureID++, texinfo._xpos, texinfo._ypos, texinfo._width, texinfo._height);

	// Get the pack info
	auto it = m_PackInfo.find(packID);
	
	// Calculate the uvs for the texture
	tex._u = (float)tex._xpos / (float)it->second._width;
	tex._v = (float)tex._ypos / (float)it->second._height;
	tex._uvwidth = (float)tex._width / (float)it->second._width;
	tex._uvheight = (float)tex._height / (float)it->second._height;

	// Insert the texture into the map
	m_Textures.insert(std::pair<uint16_t, uint16_t>(tex._tex_id, packID));

	// Insert the texture info into the other map
	m_TexInfo.insert(std::pair<uint16_t, TextureDetails>(tex._tex_id, tex));

	return tex;
}

TextureDetails RenderSystem::CreateTextureDetails(TextureDetails texinfo, ID3D11Texture2D* texture, uint32_t width, uint32_t height)
{
	TextureDetails tex(0, 0, 0, width, height);

	// Create a new pack for this texture
	uint16_t packID = CreatePack(texture, width, height);
	
	// Now create a texture details
	return CreateTextureDetails(tex, packID);
}

void RenderSystem::OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename)
{
	char* compileErrors;
	unsigned long bufferSize, i;
	ofstream fout;


	// Get a pointer to the error message text buffer.
	compileErrors = (char*)(errorMessage->GetBufferPointer());

	// Get the length of the message.
	bufferSize = errorMessage->GetBufferSize();

	// Open a file to write the error message to.
	fout.open("shader-error.txt");

	// Write out the error message.
	for(i=0; i<bufferSize; i++)
	{
		fout << compileErrors[i];
	}

	// Close the file.
	fout.close();

	// Release the error message.
	errorMessage->Release();
	errorMessage = 0;

	// Pop a message up on the screen to notify the user to check the text file for compile errors.
	log_sxerror("RenderSystem", "Error compiling shader.  Check shader-error.txt for message.");

	return;
}

void RenderSystem::setShaderPath(const std::string &path) {
    shaderPath = path;
}

const std::string &RenderSystem::getShaderPath() {
    return shaderPath;
}

}