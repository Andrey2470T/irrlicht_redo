#include "Texture.h"
#include "Render/Common.h"

namespace render
{

Texture::Texture(const std::string &_name, u32 _width, u32 _height, img::PixelFormat _format,
	const TextureSettings &_texSettings)
	: name(_name), width(_width), height(_height), format(_format), texSettings(_texSettings)
{}
	
Texture::~Texture()
{
	glDeleteTextures(1, &texID);
    TEST_GL_ERROR();
}

bool Texture::operator==(const Texture *other) const
{
    return texID == other->texID;
}

}
