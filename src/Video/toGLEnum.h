#pragma once

#include <array>
#include "Common.h"

namespace render
{
	
enum TextureWrapping
{
	TW_REPEAT = 0,
	TW_CLAMP_TO_EDGE,
	TW_CLAMP_TO_BORDER,
	TW_MIRRORED_REPEAT,
	TW_MIRROR_CLAMP_TO_EDGE,
	TW_COUNT
};

enum TextureMinFilter
{
	TMF_NEAREST = 0,
	TMF_LINEAR,
	TMF_NEAREST_MIPMAP_NEAREST,
	TMF_LINEAR_MIPMAP_NEAREST,
	TMF_NEAREST_MIPMAP_LINEAR,
	TMF_LINEAR_MIPMAP_LINEAR,
	TMF_COUNT
};

enum TextureMagFilter
{
	TMAGF_NEAREST = 0,
	TMAGF_LINEAR,
	TMAGF_COUNT
};

extern std::array<GLenum, TMF_COUNT> toGLMinFilter;
extern std::array<GLenum, TMAGF_COUNT> toGLMagFilter;
extern std::array<GLenum, TW_COUNT> toGLWrap;

}
