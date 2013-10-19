// Texture Resource Implementation File

#include "TextureResource.h"
#include "ResourceManager.h"

using namespace Timewhale;
using namespace std;

twTexture::twTexture(uint16_t id)
	: twResource(), _width(0), _height(0), _texUID(id)
{
	
}

twTexture::twTexture(uint16_t id, uint32_t width, uint32_t height)
	: twResource(), _width(width), _height(height), _texUID(id)
{
}

uint32_t twTexture::Width()
{
	return _width;
}

uint32_t twTexture::Height()
{
	return _height;
}

uint32_t twTexture::TextureUID()
{
	return _texUID;
}

Texture::Texture(const string& name)
	: IResource(name, TEXTURE)
{
	//_tex = nullptr;

	if (!_isReady){
	}
	// Make sure what we loaded was a texture
	if(_tex = dynamic_cast<twTexture*>(_rsc))
	{
			
	}
	else // Error bitches!
	{
		// What we loaded was not a texture!
		// Here is where we would unload it if we wanted
	}
}

Texture::Texture(uint32_t tag)
	: IResource(tag, TEXTURE)
{
	//_tex = nullptr;

	if(_tex = dynamic_cast<twTexture*>(_rsc))
	{
			
	}
	else // Error bitches!
	{
		// What we loaded was not a texture!
		// Here is where we would unload it if we wanted
	}
}

Texture::Texture(const Texture& other)
	: IResource(other)
{
	*this = other;
}

Texture& Texture::operator=(const Texture& other)
{
	if (&other == this)
		return *this;

	_tex = other._tex;
	
	return *this;
}

const uint32_t Texture::Width() 
{
	if(!_isReady)
	{
		if(_getReady())
		{
			_tex = dynamic_cast<twTexture*>(_rsc);
			_isReady = true;
		}
	}
	return _tex != nullptr ? _tex->Width() : 0;
}

const uint32_t Texture::Height() 
{
	if(!_isReady)
	{
		if(_getReady())
		{
			_tex = dynamic_cast<twTexture*>(_rsc);
			_isReady = true;
		}
	}
	return _tex != nullptr ? _tex->Height() : 0;
}

const uint32_t Texture::TextureUID()
{	
	if(!_isReady)
	{
		if(_getReady())
		{
			_tex = dynamic_cast<twTexture*>(_rsc);
			_isReady = true;
		}
	}
	return _tex != nullptr ? _tex->TextureUID() : 0;
}