#pragma once

#include <SDL_video.h>
#include <GL/glew.h>

#include "BasicIncludes.h"
#include <optional>

namespace main
{

enum OpenGLType : u8
{
    OGL_TYPE_DESKTOP,
    OGL_TYPE_ES,
    OGL_TYPE_WEB
};

struct OpenGLVersion
{
    SDL_GLprofile Profile;

    u8 Major;
    u8 Minor;
public:
    OpenGLVersion(OpenGLType type, std::optional<u8> major=std::nullopt, std::optional<u8> minor=std::nullopt)
    {
        switch (type) {
        case OGL_TYPE_DESKTOP:
            Profile = SDL_GL_CONTEXT_PROFILE_COMPATIBILITY;
            Major = major ? major.value() : 3;
            Minor = minor ? minor.value() : 2;
        case OGL_TYPE_ES:
        case OGL_TYPE_WEB:
            Profile = SDL_GL_CONTEXT_PROFILE_ES;
            Major = major ? major.value() : 2;
            Minor = minor ? minor.value() : 0;
        }
    }

    bool isVersionAtLeast(u8 major, u8 minor)
    {
        if (Major < major)
            return false;
        if (Major > major)
            return true;
        return Minor >= minor;
    }
};

struct GLParameters
{
    const u8 *version;
    const u8 *vendor;

    s32 maxTextureUnits;
    s32 maxColorAttachments = 1;

    s32 maxTextureSize;
    s32 maxCubeMapTextureSize;

    s32 maxIndexCount;
    f32 maxTextureLODBias;

    s32 maxAnisotropy;
    s32 maxLabelLength;

    s32 maxSamples;

    std::array<f32, 2> dimAliasedLine;
    std::array<f32, 2> dimAliasedPoint;

    bool anisotropicFilterSupported;
    bool lodBiasSupported;
    bool blendMinMaxSupported;
    bool textureMultisampleSupported;
    bool khrDebugSupported;

    GLParameters(OpenGLType glType, OpenGLVersion &glVersion);
};

}
