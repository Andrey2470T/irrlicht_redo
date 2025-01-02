#include "toGLEnum.h"

namespace render
{

std::array<GLenum, TMF_COUNT> toGLMinFilter = {
    GL_NEAREST,
    GL_LINEAR,
    GL_NEAREST_MIPMAP_NEAREST,
    GL_LINEAR_MIPMAP_NEAREST,
    GL_NEAREST_MIPMAP_LINEAR,
    GL_LINEAR_MIPMAP_LINEAR
};

std::array<GLenum, TMAGF_COUNT> toGLMagFilter = {
    GL_NEAREST,
    GL_LINEAR
};

std::array<GLenum, TW_COUNT> toGLWrap = {
    GL_REPEAT,
    GL_CLAMP_TO_EDGE,
    GL_CLAMP_TO_BORDER,
    GL_MIRRORED_REPEAT,
    GL_MIRROR_CLAMP_TO_EDGE
};

std::array<GLenum, BO_COUNT> toGLBlendOp = {
    GL_FUNC_ADD,
    GL_FUNC_SUBTRACT
};

std::array<GLenum, BF_COUNT> toGLBlendFunc = {
    GL_ZERO,
    GL_ONE,
    GL_SRC_COLOR,
    GL_ONE_MINUS_SRC_COLOR,
    GL_DST_COLOR,
    GL_ONE_MINUS_DST_COLOR,
    GL_SRC_ALPHA,
    GL_ONE_MINUS_SRC_ALPHA,
    GL_DST_ALPHA,
    GL_ONE_MINUS_DST_ALPHA
};

std::array<GLenum, PT_COUNT> toGLPrimitiveType = {
    GL_POINTS,
    GL_LINE_STRIP,
    GL_LINE_LOOP,
    GL_LINES,
    GL_TRIANGLE_STRIP,
    GL_TRIANGLE_FAN,
    GL_TRIANGLES,
    GL_POINTS
};

std::array<GLenum, static_cast<std::size_t>(BasicType::COUNT)> toGLType = {
    GL_UNSIGNED_BYTE,
    GL_UNSIGNED_SHORT,
    GL_UNSIGNED_INT,
    GL_UNSIGNED_INT,
    GL_BYTE,
    GL_SHORT,
    GL_INT,
    GL_INT,
    GL_FLOAT,
    GL_DOUBLE
};

std::array<GLenum, CF_COUNT> toGLCompareFunc = {
    GL_LESS,
    GL_EQUAL,
    GL_LEQUAL,
    GL_GREATER,
    GL_NOTEQUAL,
    GL_GEQUAL,
    GL_ALWAYS
};

std::array<GLenum, CM_COUNT> toGLCullMode = {
    GL_BACK,
    GL_FRONT,
    GL_FRONT_AND_BACK
};

std::array<GLenum, SO_COUNT> toGLStencilOp = {
    GL_KEEP,
    GL_ZERO,
    GL_REPLACE,
    GL_INVERT
};

}
