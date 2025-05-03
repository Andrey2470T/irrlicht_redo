#include "Texture.h"

namespace render
{

Texture::Texture(const std::string &_name, u32 _width, u32 _height, img::PixelFormat _format,
	const TextureSettings &_texSettings)
	: name(_name), width(_width), height(_height), format(_format), texSettings(_texSettings)
{}
	
Texture::~Texture()
{
	glDeleteTextures(1, &texID);
}

}
