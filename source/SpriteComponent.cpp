#include <cassert>
#include "SpriteComponent.h"
#include "TWLogger.h"
#include "TextureResource.h"


namespace Timewhale {

	SpriteComponent::SpriteComponent()
		:IComponent(),
		mTexture(0), // This will work but your code might only produce fail whales now.... talk to me if youre reading this - Ian
		mLayer(0.0f),
		mFrameInfo(),
		mFrameDuration(1),
		mTickCounter(0),
		mCurrentFrame(0),
		mHorizontalFrameMax(1),
		mVerticalFrameMax(1),
		mAnimLooping(false),
		mVisible(true),
		mLoopDir(SpriteAnimDir::Stopped),
		r(0.0f), g(0.0f), b(0.0f)
	{
		log_sxinfo("Sprite", "New Sprite Created. ID: %10d", getID().id);
	}

	void SpriteComponent::SetSpriteInfo(const SpriteDesc& desc) {
		// log_sxinfo("Sprite", "Setting Sprite Info:\n\tFrameW: %5d FrameH: %5d\n\tSheetW: %5d SheetH: %5d\n\tFrames: %5d\n\tLayer: %5d\n\tDuration: %5d",
		// 	desc.SpriteFrame.Width, desc.SpriteFrame.Height,
		// 	desc.SpriteSheet.Width, desc.SpriteFrame.Height,
		// 	desc.SpriteSheet.NumFrames, desc.Layer,
		// 	desc.SpriteFrame.Duration);
		mLayer = desc.Layer;
		mFrameInfo = Rectangle(
			desc.SpriteSheet.Width,
			desc.SpriteSheet.Height,
			desc.SpriteFrame.Width, 
			desc.SpriteFrame.Height);
		mFrameDuration = desc.SpriteFrame.Duration;
		mCurrentFrame = 0;
		mHorizontalFrameMax = desc.SpriteSheet.NumHorizontalFrames;
		mVerticalFrameMax = desc.SpriteSheet.NumVerticalFrames;

		mAnimLooping = desc.Looping;
		mLoopDir = desc.Direction;

		r = desc.r; g = desc.g; b = desc.b;
		//Make sure we dont try to do more frames than we have?
		assert(mFrameMax * mFrameInfo.width <= mFrameInfo.x); 
	}

	void SpriteComponent::SetSpriteSheet(Texture sprite_sheet) {
		assert(sprite_sheet.TextureUID() != 0);
		// log_sxinfo("Sprite", "Setting Sprite Sheet:\n\tTextureUID: %10d", sprite_sheet.TextureUID());

		if(mTexture.TextureUID() == sprite_sheet.TextureUID()) return;
		mTexture = sprite_sheet;
	}

	void SpriteComponent::SetLayer(float layer) {
		// log_sxinfo("Sprite", "Setting Sprite Layer:\n\tLayer: %5d", layer);
		this->mLayer = layer;
	}

	void SpriteComponent::SetFrameTickDuration(const size_t tpf) {
		// log_sxinfo("Sprite", "Setting Sprite Duration:\n\tDuration: %5d", tpf);
		mFrameDuration = tpf == 0 ? 1 : tpf;
	}

	void SpriteComponent::SetCurrentFrame(const size_t frame) {
		// log_sxinfo("Sprite", "Setting Sprite Frame:\n\tFrame: %5d", frame);
		mCurrentFrame = frame >= (mHorizontalFrameMax * mVerticalFrameMax) ? 0 : frame;
	}

	void SpriteComponent::SetFrameInfo(const float _fW, const float _fH, const float _sW, const float _sH) {
		// log_sxinfo("Sprite", "Setting Sprite Frame Info:\n\tFrameW: %5d FrameH: %5d", _fW, _fH);
		// log_sxinfo("Sprite", "Setting Sprite Sheet Info:\n\tSheetW: %5d SheetH: %5d", _sW, _sH);
		assert( _fW <= _sW);
		assert( _fH <= _sH);
		mFrameInfo = Rectangle(
			_sW, _sH,
			_fW, _fH);
	}

	void SpriteComponent::SetMaxFrames(const size_t horizontalMax, const size_t verticalMax) {
		// log_sxinfo("Sprite", "Setting Sprite Max Frames:\n\tMaxFrames: %5d", max);
		mHorizontalFrameMax = horizontalMax;
		mVerticalFrameMax = verticalMax;
	}

	void SpriteComponent::SetLooping(const bool looping) {
		// log_sxinfo("Sprite", "Setting Sprite Anim Looping:\n\tLooping: %c", BoolToChar(looping));
		mAnimLooping = looping;
	}

	void SpriteComponent::SetLoopDirection(const SpriteAnimDir direction) {
		// log_sxinfo("Sprite", "Setting Sprite Anim Direction:\n\tDirection: %d", direction);
		mLoopDir = direction;
	}

	void SpriteComponent::Show() {
		mVisible = true;
	}
	void SpriteComponent::Hide() {
		mVisible = false;
	}

	void SpriteComponent::SetColor(const float _r, const float _g, const float _b) {
		r = _r; g = _g; b = _b;
	}

	Texture& SpriteComponent::GetSpriteSheet() {
		return mTexture;
	}
	size_t SpriteComponent::GetSpriteSheetID() {
		return mTexture.TextureUID();
	}
	const float SpriteComponent::GetLayer() const {
		return mLayer;
	}
	const size_t SpriteComponent::GetFrameTickDuration() const {
		return mFrameDuration;
	}
	const size_t SpriteComponent::GetCurrentFrame() const {
		return mCurrentFrame;
	}
	const float SpriteComponent::GetFrameWidth() const {
		return mFrameInfo.width;
	}
	const float SpriteComponent::GetFrameHeight() const {
		return mFrameInfo.height;
	}
	const float SpriteComponent::GetSheetWidth() const {
		return mFrameInfo.x;
	}
	const float SpriteComponent::GetSheetHeight() const {
		return mFrameInfo.y;
	}
	const size_t SpriteComponent::GetMaxFrames() const {
		return mHorizontalFrameMax * mVerticalFrameMax;
	}
	const size_t SpriteComponent::GetHorizontalMaxFrames() const {
		return mHorizontalFrameMax;
	}
	const size_t SpriteComponent::GetVerticalMaxFrames() const {
		return mVerticalFrameMax;
	}
	const bool SpriteComponent::IsLooping() const {
		return mAnimLooping;
	}
	const SpriteAnimDir SpriteComponent::GetLoopDirection() const {
		return mLoopDir;
	}
	const bool SpriteComponent::IsVisible() const {
		return mVisible;
	}
	const float SpriteComponent::GetR() const {
		return r;
	}
	const float SpriteComponent::GetG() const {
		return g;
	}
	const float SpriteComponent::GetB() const {
		return b;
	}

	void SpriteComponent::Update() {
		++mTickCounter;
		if(mTickCounter == mFrameDuration) {
			mTickCounter = 0;
			mCurrentFrame += mLoopDir;
			if(mLoopDir == SpriteAnimDir::Forward) {
				if(mCurrentFrame >= (mHorizontalFrameMax * mVerticalFrameMax)) {
					mCurrentFrame = mAnimLooping ? 0 : (mHorizontalFrameMax * mVerticalFrameMax)-1;
				}
			} else if (mLoopDir == SpriteAnimDir::Reverse) {
				if(mCurrentFrame <= 0) {
					mCurrentFrame = mAnimLooping ? (mHorizontalFrameMax * mVerticalFrameMax)-1 : 0;
				}
			}

		}

	}
}
