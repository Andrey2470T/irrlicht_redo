#include "TextureFormats.h"

namespace render
{

toGLFormatConverter[TF_A1R5G5B5] = {GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV}; // WARNING: may not be renderable
toGLFormatConverter[TF_R5G6B5] = {GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5};              // GL_RGB565 is an extension until 4.1
toGLFormatConverter[TF_R8G8B8] = {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE};                    // WARNING: may not be renderable
toGLFormatConverter[TF_A8R8G8B8] = {GL_RGBA8, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV};
toGLFormatConverter[TF_R16F] = {GL_R16F, GL_RED, GL_HALF_FLOAT};
toGLFormatConverter[TF_G16R16F] = {GL_RG16F, GL_RG, GL_HALF_FLOAT};
toGLFormatConverter[TF_A16B16G16R16F] = {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT};
toGLFormatConverter[TF_R32F] = {GL_R32F, GL_RED, GL_FLOAT};
toGLFormatConverter[TF_G32R32F] = {GL_RG32F, GL_RG, GL_FLOAT};
toGLFormatConverter[TF_A32B32G32R32F] = {GL_RGBA32F, GL_RGBA, GL_FLOAT};
toGLFormatConverter[TF_R8] = {GL_R8, GL_RED, GL_UNSIGNED_BYTE};
toGLFormatConverter[TF_R8G8] = {GL_RG8, GL_RG, GL_UNSIGNED_BYTE};
toGLFormatConverter[TF_R16] = {GL_R16, GL_RED, GL_UNSIGNED_SHORT};
toGLFormatConverter[TF_R16G16] = {GL_RG16, GL_RG, GL_UNSIGNED_SHORT};
toGLFormatConverter[TF_D16] = {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT};
toGLFormatConverter[TF_D32] = {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT}; // WARNING: may not be renderable (?!)
toGLFormatConverter[TF_D24S8] = {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};

}
