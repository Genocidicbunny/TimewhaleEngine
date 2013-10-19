// Texture Resource Header File

#pragma once

#ifndef __TEXTURERESOURCE_H
#define __TEXTURERESOURCE_H

#include <memory>

#include "IResource.h"

namespace Timewhale
{
	class twTexture : public twResource
	{

	protected:
		uint32_t _width;
		uint32_t _height;
		uint16_t _texUID;

	public:	
		twTexture(uint16_t id);
		twTexture(uint16_t id, uint32_t width, uint32_t height);
		uint32_t Width();
		uint32_t Height();
		uint32_t TextureUID();
	};

	class Texture : IResource
	{
		twTexture* _tex;

	public:
		Texture(const std::string& name);
		Texture(uint32_t tag);
		Texture(const Texture& other);
		Texture& operator=(const Texture& other);

		const uint32_t Width();
		const uint32_t Height();
		const uint32_t TextureUID();
	};
}

#endif