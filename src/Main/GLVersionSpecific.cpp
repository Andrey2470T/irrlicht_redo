#include "GLVersionSpecific.h"

namespace main
{

GLParameters::GLParameters(OpenGLType glType, OpenGLVersion &glVersion)
{
    version = glGetString(GL_VERSION);
    vendor = glGetString(GL_VENDOR);

    if (glType == OGL_TYPE_DESKTOP) {
        anisotropicFilterSupported = glVersion.isVersionAtLeast(4, 6) || GLEW_ARB_texture_filter_anisotropic || GLEW_EXT_texture_filter_anisotropic;
        lodBiasSupported = true;
        blendMinMaxSupported = true;
        textureMultisampleSupported = true;
        khrDebugSupported = glVersion.isVersionAtLeast(4, 6) || GLEW_KHR_debug;

        if (anisotropicFilterSupported)
            glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &maxAnisotropy);

        glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);
        glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndexCount);

        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &dimAliasedLine[0]);
        dimAliasedPoint[0] = 1.0f;
        dimAliasedPoint[1] = 1.0f;
    }
    else {
        anisotropicFilterSupported = GLEW_EXT_texture_filter_anisotropic;
        lodBiasSupported = GLEW_EXT_texture_lod_bias;
        blendMinMaxSupported = glVersion.Major >= 3  || GLEW_EXT_blend_minmax;
        textureMultisampleSupported = glVersion.isVersionAtLeast(3, 1);
        khrDebugSupported = GLEW_KHR_debug;

        if (anisotropicFilterSupported)
            glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &maxAnisotropy);

        if (glVersion.isVersionAtLeast(3, 0) || GLEW_EXT_draw_buffers)
            glGetIntegerv(GL_MAX_COLOR_ATTACHMENTS, &maxColorAttachments);

        if (glVersion.Major >= 3 || GLEW_EXT_draw_range_elements)
            glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &maxIndexCount);

        glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, &dimAliasedLine[0]); // NOTE: this is not in the OpenGL ES 2.0 spec...
        glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, &dimAliasedPoint[0]);
    }

    glGetIntegerv(GL_MAX_TEXTURE_UNITS, &maxTextureUnits);
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
    glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &maxCubeMapTextureSize);
    glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &maxTextureLODBias);

    if (khrDebugSupported)
        glGetIntegerv(GL_MAX_LABEL_LENGTH, &maxLabelLength);

    glGetIntegerv(GL_MAX_SAMPLES, &maxSamples);
}

}
