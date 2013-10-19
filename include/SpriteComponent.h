#pragma once

#ifndef __TIMEWHALE_SPRITECOMPONENT_H_
#define __TIMEWHALE_SPRITECOMPONENT_H_
#include "ECS.h"
#include "WhaleMath.h"
#include "TextureResource.h"
#include "TransformComponent.h"

namespace Timewhale {

	struct SpriteLayer {
	public:
		static const size_t LayerMax = (size_t)(1 << 4);

		size_t layer;
		SpriteLayer(const size_t _l) 
			:layer(_l > LayerMax ? 0 : _l)
		{
		}

		inline const bool operator==(const SpriteLayer& other) const {
			return layer == other.layer;
		}
		inline const bool operator!=(const SpriteLayer& other) const {
			return layer != other.layer;
		}
		inline const bool operator<(const SpriteLayer& other) const {
			return layer < other.layer;
		}
		inline const bool operator>(const SpriteLayer& other) const {
			return layer > other.layer;
		}
		inline const bool operator>=(const SpriteLayer& other) const {
			return layer >= other.layer;
		}
		inline const bool operator<=(const SpriteLayer& other) const {
			return layer <= other.layer;
		}
		inline const size_t operator*() const {
			return layer;
		}
	};

	enum SpriteAnimDir {
		Reverse= -1,
		Stopped = 0,
		Forward = 1
	};

	struct SpriteDesc {
		float Layer;
		struct SpriteFrame {
			float Width;
			float Height;
			size_t Duration;
		} SpriteFrame;
		struct SpriteSheet {
			float Width;
			float Height;
			size_t NumHorizontalFrames;
			size_t NumVerticalFrames;
		} SpriteSheet;
		bool Looping;
		SpriteAnimDir Direction;
		float r, g, b;
	};

	class SpriteComponent : public IComponent {
	private:
		Texture mTexture; //spritesheet
		float mLayer; //layer
		Rectangle mFrameInfo; //x/y store sheet info, width/height store frame info
		size_t mFrameDuration; //ticks per sprite frame
 		size_t mTickCounter;   //current tick counter
		size_t mCurrentFrame;  //current frame counter
		size_t mHorizontalFrameMax;   //maximum frames in spritesheet (roll over after this)
		size_t mVerticalFrameMax;
		bool mAnimLooping;
		SpriteAnimDir mLoopDir;
		bool mVisible;
		float r, g, b;   // Stores team color data

	public:
		SpriteComponent();
		void SetSpriteInfo(const SpriteDesc& desc);
		void SetSpriteSheet(Texture sprite_sheet);
		void SetLayer(float layer);
		void SetFrameTickDuration(const size_t tpf);
		void SetCurrentFrame(const size_t frame);
		void SetFrameInfo(const float _fW, const float _fH, const float _sW, const float _sH);
		void SetMaxFrames(const size_t horizontalMax, const size_t verticalMax = 1);
		void SetLooping(const bool looping);
		void SetLoopDirection(const SpriteAnimDir direction);
		void Show();
		void Hide();
		void SetColor(const float r, const float g, const float b);

		Texture& GetSpriteSheet();
		size_t GetSpriteSheetID();
		const float GetLayer() const;
		const size_t GetFrameTickDuration() const;
		const size_t GetCurrentFrame() const;
		const float GetFrameWidth() const;
		const float GetFrameHeight() const;
		const float GetSheetWidth() const;
		const float GetSheetHeight() const;
		const size_t GetMaxFrames() const;
		const size_t GetHorizontalMaxFrames() const;
		const size_t GetVerticalMaxFrames() const;
		const bool IsLooping() const;
		const SpriteAnimDir GetLoopDirection() const;
		const bool IsVisible() const;
		const float GetR() const;
		const float GetG() const;
		const float GetB() const;

		virtual void Update();
	};

	REGISTER_COMPONENT_WFLAGS(SpriteComponent, "SpriteComponent", CREG_FLAGS_ENGCOMP);
	REGISTER_DEPENDENCY(SpriteComponent, Transform2D);
	static const TypeID SpriteTypeID(ctid<SpriteComponent>());

	typedef std::shared_ptr<SpriteComponent> SpriteComponentPtr;
}

#endif