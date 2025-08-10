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

enum PrimitiveType
{
	PT_POINTS = 0,
	PT_LINE_STRIP,
	PT_LINE_LOOP,
	PT_LINES,
	PT_TRIANGLE_STRIP,
	PT_TRIANGLE_FAN,
	PT_TRIANGLES,
	PT_POINT_SPRITES,
	PT_COUNT
};

enum CompareFunc
{
	CF_LESS = 0,
	CF_EQUAL,
	CF_LEQUAL,
	CF_GREATER,
	CF_NOTEQUAL,
	CF_GEQUAL,
	CF_ALWAYS,
	CF_COUNT
};

enum CullMode
{
	CM_BACK = 0,
	CM_FRONT,
	CM_FRONT_AND_BACK,
	CM_COUNT
};

enum StencilOp
{
	SO_KEEP = 0,
	SO_ZERO,
	SO_REPLACE,
	SO_INVERT,
	SO_COUNT
};

enum PolygonMode
{
    PM_FILL = 0,
    PM_POINT,
    PM_LINE,
    PM_COUNT
};

extern std::array<GLenum, TMF_COUNT> toGLMinFilter;
extern std::array<GLenum, TMAGF_COUNT> toGLMagFilter;
extern std::array<GLenum, TW_COUNT> toGLWrap;
extern std::array<GLenum, BO_COUNT> toGLBlendOp;
extern std::array<GLenum, BF_COUNT> toGLBlendFunc;
extern std::array<GLenum, PT_COUNT> toGLPrimitiveType;
extern std::array<GLenum, static_cast<std::size_t>(BasicType::COUNT)> toGLType;
extern std::array<GLenum, CF_COUNT> toGLCompareFunc;
extern std::array<GLenum, CM_COUNT> toGLCullMode;
extern std::array<GLenum, SO_COUNT> toGLStencilOp;
extern std::array<GLenum, PM_COUNT> toGLPolygonMode;

}
