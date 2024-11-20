#pragma once

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

enum BlendOp
{
	BO_ADD = 0,
	BO_SUBTRACT,
	BO_MULTIPLY, // Not supported in OpenGL, but used in ImageModifier
	BO_COUNT
};

enum BlendFunc
{
	BF_ZERO = 0,
	BF_ONE,
	BF_SRC_COLOR,
	BF_ONE_MINUS_SRC_COLOR,
	BF_DST_COLOR,
	BF_ONE_MINUS_DST_COLOR,
	BF_SRC_ALPHA,
	BF_ONE_MINUS_SRC_ALPHA,
	BF_DST_ALPHA,
	BF_ONE_MINUS_DST_ALPHA,
	BF_COUNT
};

extern std::array<GLenum, TMF_COUNT> toGLMinFilter;
extern std::array<GLenum, TMAGF_COUNT> toGLMagFilter;
extern std::array<GLenum, TW_COUNT> toGLWrap;
extern std::array<GLenum, BO_COUNT> toGLBlendOp;
extern std::array<GLenum, BF_COUNT-1> toGLBlendFunc;

}
