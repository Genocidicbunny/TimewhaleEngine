#pragma once

#ifndef __Render_System
#define __Render_System

#include <Windows.h>
#include <map>
#include <vector>
#include <memory>
#include <chrono>

#define TWDIRECTX

#ifdef TWDIRECTX
#include <d3d11.h>

#include "D3DManager.h"
#include "SpriteShader.h"
#include "Sprite.h"
#include "OSB.h"
#include "Camera.h"
#include "UIObject.h"
typedef Timewhale::D3DManager Renderer;
#else 
#ifdef TWOPENGL
#include "OGLRenderer.h"
typedef Timewhale::OGLRenderer Renderer;
#endif
#endif

// Globals
const bool FULL_SCREEN = false;
const bool VSYNC_ENABLED = true;
const float SCREEN_DEPTH = 1000.0f;
const float SCREEN_NEAR = 0.1f;

namespace Timewhale {

extern std::string shaderPath;

struct TextureDetails
{
	uint16_t _tex_id;
	uint32_t _xpos;
	uint32_t _ypos;
	uint32_t _width;
	uint32_t _height;
	float    _u;
	float    _v;
	float    _uvwidth;
	float    _uvheight;

	TextureDetails(uint16_t tex_id, uint32_t xpos, uint32_t ypos, uint32_t width, uint32_t height)
		: _tex_id(tex_id), _xpos(xpos), _ypos(ypos), _width(width), _height(height) {}
};

struct PackDetails
{
	uint16_t _pack_id;
	uint32_t _width;
	uint32_t _height;

	PackDetails(uint16_t pack_id, uint32_t width, uint32_t height)
		: _pack_id(pack_id), _width(width), _height(height) {}
};

class RenderSystem
{
	friend class RocketRender;
	friend class DXTexture;
	friend class SpriteBatch;
	friend class SpriteShader;
	friend class UIObject;
	friend class UIShader;
public:
    inline static std::shared_ptr<RenderSystem> const get() {
        return sSystem;
    }

    static std::shared_ptr<RenderSystem> const create();

    RenderSystem(void);
    ~RenderSystem(void);

    bool Initialize(int width, int height, HWND hwnd);
	bool Resize(int windowWidth, int windowHeight);
    void Shutdown();
    bool Draw();
	void BufferScenes(const SceneRenderData &, const size_t);

    void setShaderPath(const std::string &);
    const std::string &getShaderPath();

	TextureDetails CreateTextureDetails(TextureDetails texinfo, uint16_t packID);
	TextureDetails CreateTextureDetails(TextureDetails texinfo, ID3D11Texture2D* texture, uint32_t width, uint32_t height);
	PackDetails CreatePack(uint8_t* data, uint32_t len);
	uint16_t CreatePack(ID3D11Texture2D* texture, uint32_t width, uint32_t height);
	void OutputShaderErrorMessage(ID3D10Blob* errorMessage, HWND hwnd, WCHAR* shaderFilename);
    
private:
    bool Render();

	Renderer* mRenderer;
	uint16_t currentPackID, currentTextureID;

	HWND m_hwnd;

    static std::shared_ptr<RenderSystem> sSystem;

	// Sprite batch object
	SpriteBatch* m_SpriteBatch;
	// Shader used to render our sprites
	SpriteShader* m_SpriteShader;

	// Contains pointers to each texture pack
	std::map<uint16_t, DXTexture*> m_Packs;
	// Contains information on which texture is in each pack
	// Goes Texture ID - Pack ID
	std::map<uint16_t, uint16_t> m_Textures;

	// Stores pack information
	std::map<uint16_t, PackDetails> m_PackInfo;
	// Stores texture information
	std::map<uint16_t, TextureDetails> m_TexInfo;	

	// Contains each sprite, indexed by which texture it uses
    std::vector<std::pair<uint16_t, std::vector<SpriteInstanceData>>> m_Sprites;

	// Camera Object
	Camera* m_Camera;

};

    typedef std::shared_ptr<RenderSystem> RenderSystemPtr;

}

#endif
