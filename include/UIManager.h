/*
 *
 * UI Manager class
 *
 *
*/
#pragma once
#ifndef __UI_MANAGER
#define __UI_MANAGER

#include "InstanceID.h"
#include "Rocket\Core.h"
#include "Rocket\Controls.h"
#include "DXTexture.h"
#include "UIObject.h"
#include "UIShader.h"
#include <memory>
#include <unordered_map>

#ifdef _DEBUG
#pragma comment(lib, "RocketCore_d")
#pragma comment(lib, "RocketControls_d")
#else
#pragma comment(lib, "RocketCore")
#pragma comment(lib, "RocketControls")
#endif

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

namespace Timewhale {

    class RocketSystem : public Rocket::Core::SystemInterface {
        // Get the number of seconds elapsed since the start of the application.
        virtual float GetElapsedTime();

        // Translate the input string into the translated string.
        virtual int TranslateString(Rocket::Core::String& translated, const Rocket::Core::String& input);

        // Log the specified message.
        virtual bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message);
    };

    class RocketRender : public Rocket::Core::RenderInterface {
	private:
		// Shader for DX11
		UIShader* m_UIShader;
	public:
		RocketRender();
        virtual ~RocketRender();

		// Called to initialize the rocket renderer
		bool Initialize();

        // Called by Rocket when it wants to render geometry that the application does not wish to optimise.
        virtual void RenderGeometry(
            Rocket::Core::Vertex* vertices, 
            int num_vertices, 
            int* indices, 
            int num_indices, 
            Rocket::Core::TextureHandle texture, 
            const Rocket::Core::Vector2f& translation);

        // Called by Rocket when it wants to compile geometry it believes will be static for the forseeable future.
        virtual Rocket::Core::CompiledGeometryHandle CompileGeometry(
            Rocket::Core::Vertex* vertices, 
            int num_vertices, 
            int* indices, 
            int num_indices, 
            Rocket::Core::TextureHandle texture);

        // Called by Rocket when it wants to render application-compiled geometry.
        virtual void RenderCompiledGeometry(
            Rocket::Core::CompiledGeometryHandle geometry, 
            const Rocket::Core::Vector2f& translation);

        // Called by Rocket when it wants to release application-compiled geometry.
        virtual void ReleaseCompiledGeometry(Rocket::Core::CompiledGeometryHandle geometry);

        // Called by Rocket when it wants to enable or disable scissoring to clip content.
        virtual void EnableScissorRegion(bool enable);

        // Called by Rocket when it wants to change the scissor region.
        virtual void SetScissorRegion(int x, int y, int width, int height);

        // Called by Rocket when a texture is required by the library.
        virtual bool LoadTexture(
            Rocket::Core::TextureHandle& texture_handle,
            Rocket::Core::Vector2i& texture_dimensions,
            const Rocket::Core::String& source);

        // Called by Rocket when a texture is required to be built from an internally-generated sequence of pixels.
        virtual bool GenerateTexture(Rocket::Core::TextureHandle& texture_handle,
            const Rocket::Core::byte* source,
            const Rocket::Core::Vector2i& source_dimensions);

        // Called by Rocket when a loaded texture is no longer required.
        virtual void ReleaseTexture(Rocket::Core::TextureHandle texture_handle);

        // Returns the native horizontal texel offset for the renderer.
        virtual float GetHorizontalTexelOffset();

    // Returns the native vertical texel offset for the renderer.
        virtual float GetVerticalTexelOffset();
    };

    class RocketFile : public Rocket::Core::FileInterface {
        // Opens a file.
        virtual Rocket::Core::FileHandle Open(const Rocket::Core::String& path);

        // Closes a previously opened file.
        virtual void Close(Rocket::Core::FileHandle file);

        // Reads data from a previously opened file.
        virtual size_t Read(void* buffer, size_t size, Rocket::Core::FileHandle file);

        // Seeks to a point in a previously opened file.
        virtual bool Seek(Rocket::Core::FileHandle file, long offset, int origin);

        // Returns the current position of the file pointer.
        virtual size_t Tell(Rocket::Core::FileHandle file);
    };

    class UIManager {
        typedef std::unordered_map<SceneID, Rocket::Core::Context*> ContextMap;

		//Context maps are not being used right now for simplicity
        ContextMap mContexts;
        Rocket::Core::Context* mCurrent;

        Rocket::Core::Context* mHud;
		Rocket::Core::Context* mPause;

		Rocket::Core::ElementDocument* mPauseDoc;
        Rocket::Core::ElementDocument* mHudDoc;

        bool debugVisible;
		bool pauseVisible;

        static std::shared_ptr<UIManager> sManager;

        UIManager();
    public:

		Rocket::Core::String actionsToExecute;

        inline static std::shared_ptr<UIManager> const get() {
            return sManager;
        }

        static std::shared_ptr<UIManager> const create();

        bool init();

        void update();

        void render();

        void shutdown();

        void uiKey(char key, bool up = false, uint32_t keyMods = 0);

        void uiText(char key, uint32_t keyMods = 0);

        void uiText(const std::string &str, uint32_t keyMods = 0);


        void uiMouseMove(int x, int y, uint32_t keyMods = 0);

        void uiMouseBtn(int button, bool up, uint32_t keyMods = 0);

        void uiMouseWheel(int delta, uint32_t keyMods = 0);
        
        Rocket::Core::Context* current();

        Rocket::Core::Context* debug();

		Rocket::Core::Context* pause();

		bool isHudVisible();

		bool isMouseOverUI();

		void setTextById(Rocket::Core::String id, Rocket::Core::String text);

		Rocket::Core::String* getAction(); 

        void createDebugContext();

		void createPauseContext();

        void toggleDebugContext();

		void togglePauseContext();

		void tryPause();

		void showContext();

		void hideContext();

		void showPause();

		void hidePause();

		void confirmActionReceived();

		void processEvent(Rocket::Core::Event& event, Rocket::Core::String);

		void changeStyleProperty(Rocket::Core::String element,Rocket::Core::String prop, Rocket::Core::String value);

		void setButtonFocus(Rocket::Core::String name);

		void activateCheat(int cheat);
		 
		Rocket::Core::Context* getContext();

        ~UIManager();
    };

    typedef std::shared_ptr<UIManager> UIManagerPtr;
}

#endif
