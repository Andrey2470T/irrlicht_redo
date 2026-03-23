// Copyright (C) 2015 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// 2017 modified by Michael Zeilfelder (unifying extension handlers)
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#include "GLSpecificInfo.h"

#include "SMaterial.h"
#include "fast_atof.h"
#include "Image/CColorConverter.h"
#include "Logger.h"
#include <sstream>


namespace video
{

void ExtensionHandler::initExtensions()
{
#ifdef _IRR_COMPILE_WITH_OPENGL3_
	/* OpenGL 3 & ES 3 way to enumerate extensions */
	GLint ext_count = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &ext_count);
	// clear error which is raised if unsupported
	while (glGetError() != GL_NO_ERROR) {}
		Extensions.reserve(ext_count);
	for (GLint k = 0; k < ext_count; k++) {
		auto tmp = glGetStringi(GL_EXTENSIONS, k);
		if (tmp)
			Extensions.emplace((char*)tmp);
	}
#else
	/* OpenGL 2 / ES 2 way to enumerate extensions */
	auto ext_str = glGetString(GL_EXTENSIONS);
	if (!ext_str)
		return;
	// get the extension string, chop it up
	std::istringstream ext_ss((char*)ext_str);
	std::string tmp;
	while (std::getline(ext_ss, tmp, ' '))
		Extensions.emplace(tmp);
#endif
}

static void GLAPIENTRY debugCb(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	// shader compiler can be very noisy
	if (source == GL_DEBUG_SOURCE_SHADER_COMPILER && severity == GL_DEBUG_SEVERITY_NOTIFICATION)
		return;

	ELOG_LEVEL ll = ELL_INFORMATION;
	if (severity == GL_DEBUG_SEVERITY_HIGH)
		ll = ELL_ERROR;
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM)
		ll = ELL_WARNING;
	char buf[300];
	snprintf_irr(buf, sizeof(buf), "%04x %04x %.*s", source, type, length, message);
	g_irrlogger->log("GL", buf, ll);
}

std::array<TextureFormatInfo, ECF_UNKNOWN> GLSpecificInfo::TextureFormats = {};

void GLSpecificInfo::init(bool stencilBuffer, bool enableDebug)
{
	initVersion();
	initExtensions();
	initFeatures();
	initTextureFormats();
	printTextureFormats();

	if (enableDebug) {
		if (Features.KHRDebugSupported) {
			glEnable(GL_DEBUG_OUTPUT);
			glDebugMessageCallback(debugCb, this);
		} else {
			g_irrlogger->log("GL debug extension not available");
		}
	} else {
		// don't do debug things if they are not wanted (even if supported)
		Features.KHRDebugSupported = false;
	}

	Features.StencilBuffer = stencilBuffer;
}
void GLSpecificInfo::initVersion()
{
	RendererName = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	g_irrlogger->log("Using renderer: ", RendererName.c_str(), ELL_INFORMATION);

	// print renderer information
	VendorName = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
	g_irrlogger->log("Renderer", VendorName.c_str(), ELL_INFORMATION);

	GLint major = 0, minor = 0, profile = 0;
	OpenGLSpec spec = OpenGLSpec::Core;
#ifdef _IRR_COMPILE_WITH_OPENGL3_
	glGetIntegerv(GL_MAJOR_VERSION, &major);
	glGetIntegerv(GL_MINOR_VERSION, &minor);
	glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &profile);
	// The spec is clear a context can’t be both core and compatibility at the same time.
	// However, the returned value is a mask. Ask Khronos why. -- numzero
	if (profile & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT)
		spec = OpenGLSpec::Compat;
#else
	auto version_string = reinterpret_cast<const char *>(glGetString(GL_VERSION));
	//int major = 0, minor = 0;
	spec = OpenGLSpec::ES;
	sscanf(version_string, "OpenGL ES %d.%d", &major, &minor);
#endif

	GLVersion.Spec = spec;
	GLVersion.Major = major;
	GLVersion.Minor = minor;

#ifdef _IRR_COMPILE_WITH_OPENGL3_
	if (GLVersion.Spec != OpenGLSpec::Compat) {
		auto msg = "OpenGL 3 driver requires Compatibility context";
		g_irrlogger->log(msg, ELL_ERROR);
		throw std::runtime_error(msg);
	}
	if (!isVersionAtLeast(3, 2)) {
		auto msg = "OpenGL 3 driver requires OpenGL >= 3.2";
		g_irrlogger->log(msg, ELL_ERROR);
		throw std::runtime_error(msg);
	}
#else
	if (GLVersion.Spec != OpenGLSpec::ES) {
		auto msg = "Context isn't OpenGL ES";
		g_irrlogger->log(msg, ELL_ERROR);
		throw std::runtime_error(msg);
	}
	if (!isVersionAtLeast(2, 0)) {
		auto msg = "Open GL ES 2.0 is required";
		g_irrlogger->log(msg, ELL_ERROR);
		throw std::runtime_error(msg);
	}
#endif
}

void GLSpecificInfo::initFeatures()
{
#ifdef _IRR_COMPILE_WITH_OPENGL3_
	Features.AnisotropicFilterSupported = isVersionAtLeast(4, 6) || isExtensionPresent("GL_ARB_texture_filter_anisotropic") ||
		isExtensionPresent("GL_EXT_texture_filter_anisotropic");
	Features.LODBiasSupported = true;
	Features.BlendMinMaxSupported = true;
	Features.TextureMultisampleSupported = true;
	Features.Texture2DArraySupported = GLVersion.Major >= 3 || isExtensionPresent("GL_EXT_texture_array");
	Features.KHRDebugSupported = isVersionAtLeast(4, 6) || isExtensionPresent("GL_KHR_debug");
	if (Features.KHRDebugSupported)
		Features.MaxLabelLength = GetInteger(GL_MAX_LABEL_LENGTH);

	static_assert(MATERIAL_MAX_TEXTURES <= 16, "Only up to 16 textures are guaranteed");
	Features.BlendOperation = true;
	Features.TexStorage = isVersionAtLeast(4, 2) || isExtensionPresent("GL_ARB_texture_storage");
	Features.ColorAttachment = GetInteger(GL_MAX_COLOR_ATTACHMENTS);
	Features.MaxTextureUnits = MATERIAL_MAX_TEXTURES;
	Features.MultipleRenderTarget = GetInteger(GL_MAX_DRAW_BUFFERS);

	// COGLESCoreExtensionHandler
	if (Features.AnisotropicFilterSupported)
		Features.MaxAnisotropy = GetInteger(GL_MAX_TEXTURE_MAX_ANISOTROPY);
	Features.MaxIndices = GetInteger(GL_MAX_ELEMENTS_INDICES);
	Features.MaxTextureSize = GetInteger(GL_MAX_TEXTURE_SIZE);
	if (Features.Texture2DArraySupported)
		Features.MaxArrayTextureLayers = GetInteger(GL_MAX_ARRAY_TEXTURE_LAYERS);
	glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &Features.MaxTextureLODBias);
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, Features.DimAliasedLine);
	Features.DimAliasedPoint[0] = 1.0f;
	Features.DimAliasedPoint[1] = 1.0f;
#else
	const bool MRTSupported = GLVersion.Major >= 3 || isExtensionPresent("GL_EXT_draw_buffers");
	Features.LODBiasSupported = isExtensionPresent("GL_EXT_texture_lod_bias");
	Features.AnisotropicFilterSupported = isExtensionPresent("GL_EXT_texture_filter_anisotropic");
	Features.BlendMinMaxSupported = (GLVersion.Major >= 3) || isExtensionPresent("GL_EXT_blend_minmax");
	Features.TextureMultisampleSupported = isVersionAtLeast(3, 1);
	Features.Texture2DArraySupported = GLVersion.Major >= 3 || isExtensionPresent("GL_EXT_texture_array");
	Features.KHRDebugSupported = isExtensionPresent("GL_KHR_debug");
	if (Features.KHRDebugSupported)
		Features.MaxLabelLength = GetInteger(GL_MAX_LABEL_LENGTH);

	static_assert(MATERIAL_MAX_TEXTURES <= 8, "Only up to 8 textures are guaranteed");
	Features.BlendOperation = true;
	Features.TexStorage = GLVersion.Major >= 3 || isExtensionPresent("GL_ARB_texture_storage");
	Features.ColorAttachment = 1;
	if (MRTSupported)
		Features.ColorAttachment = GetInteger(GL_MAX_COLOR_ATTACHMENTS);
	Features.MaxTextureUnits = MATERIAL_MAX_TEXTURES;
	if (MRTSupported)
		Features.MultipleRenderTarget = GetInteger(GL_MAX_DRAW_BUFFERS);

	// COGLESCoreExtensionHandler
	if (Features.AnisotropicFilterSupported)
		Features.MaxAnisotropy = GetInteger(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT);
	if (GLVersion.Major >= 3 || isExtensionPresent("GL_EXT_draw_range_elements"))
		Features.MaxIndices = GetInteger(GL_MAX_ELEMENTS_INDICES);
	Features.MaxTextureSize = GetInteger(GL_MAX_TEXTURE_SIZE);
	if (Features.Texture2DArraySupported)
		Features.MaxArrayTextureLayers = GetInteger(GL_MAX_ARRAY_TEXTURE_LAYERS);
	if (Features.LODBiasSupported)
		glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &Features.MaxTextureLODBias);
	glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, Features.DimAliasedLine);
	glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, Features.DimAliasedPoint);
#endif
}

void GLSpecificInfo::initTextureFormats()
{
#ifdef _IRR_COMPILE_WITH_OPENGL3_
	TextureFormats[ECF_A1R5G5B5] = {GL_RGB5_A1, GL_BGRA, GL_UNSIGNED_SHORT_1_5_5_5_REV}; // WARNING: may not be renderable
	TextureFormats[ECF_R5G6B5] = {GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5};              // GL_RGB565 is an extension until 4.1
	TextureFormats[ECF_R8G8B8] = {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE};                    // WARNING: may not be renderable
	TextureFormats[ECF_A8R8G8B8] = {GL_RGBA8, GL_BGRA, GL_UNSIGNED_BYTE};
	TextureFormats[ECF_R16F] = {GL_R16F, GL_RED, GL_HALF_FLOAT};
	TextureFormats[ECF_G16R16F] = {GL_RG16F, GL_RG, GL_HALF_FLOAT};
	TextureFormats[ECF_A16B16G16R16F] = {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT};
	TextureFormats[ECF_R32F] = {GL_R32F, GL_RED, GL_FLOAT};
	TextureFormats[ECF_G32R32F] = {GL_RG32F, GL_RG, GL_FLOAT};
	TextureFormats[ECF_A32B32G32R32F] = {GL_RGBA32F, GL_RGBA, GL_FLOAT};
	TextureFormats[ECF_R8] = {GL_R8, GL_RED, GL_UNSIGNED_BYTE};
	TextureFormats[ECF_R8G8] = {GL_RG8, GL_RG, GL_UNSIGNED_BYTE};
	TextureFormats[ECF_R16] = {GL_R16, GL_RED, GL_UNSIGNED_SHORT};
	TextureFormats[ECF_R16G16] = {GL_RG16, GL_RG, GL_UNSIGNED_SHORT};
	TextureFormats[ECF_A2R10G10B10] = {GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV};
	TextureFormats[ECF_D16] = {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT};
	TextureFormats[ECF_D24] = {GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT};
	TextureFormats[ECF_D32] = {GL_DEPTH_COMPONENT32, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT}; // WARNING: may not be renderable (?!)
	TextureFormats[ECF_D24S8] = {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};
#else
	if (glVersion.Major >= 3) {
		// NOTE floating-point formats may not be suitable for render targets.
		TextureFormats[ECF_A1R5G5B5] = {GL_RGB5_A1, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, CColorConverter::convert_A1R5G5B5toR5G5B5A1};
		TextureFormats[ECF_R5G6B5] = {GL_RGB565, GL_RGB, GL_UNSIGNED_SHORT_5_6_5};
		TextureFormats[ECF_R8G8B8] = {GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE};
		TextureFormats[ECF_A8R8G8B8] = {GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE, CColorConverter::convert_A8R8G8B8toA8B8G8R8};
		TextureFormats[ECF_R16F] = {GL_R16F, GL_RED, GL_HALF_FLOAT};
		TextureFormats[ECF_G16R16F] = {GL_RG16F, GL_RG, GL_HALF_FLOAT};
		TextureFormats[ECF_A16B16G16R16F] = {GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT};
		TextureFormats[ECF_R32F] = {GL_R32F, GL_RED, GL_FLOAT};
		TextureFormats[ECF_G32R32F] = {GL_RG32F, GL_RG, GL_FLOAT};
		TextureFormats[ECF_A32B32G32R32F] = {GL_RGBA32F, GL_RGBA, GL_FLOAT};
		TextureFormats[ECF_R8] = {GL_R8, GL_RED, GL_UNSIGNED_BYTE};
		TextureFormats[ECF_R8G8] = {GL_RG8, GL_RG, GL_UNSIGNED_BYTE};
		TextureFormats[ECF_A2R10G10B10] = {GL_RGB10_A2, GL_RGBA, GL_UNSIGNED_INT_2_10_10_10_REV};
		TextureFormats[ECF_D16] = {GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT};
		TextureFormats[ECF_D24] = {GL_DEPTH_COMPONENT24, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT};
		TextureFormats[ECF_D24S8] = {GL_DEPTH24_STENCIL8, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};

		// NOTE a recent (2024) revision of EXT_texture_format_BGRA8888 also
		// adds a sized format GL_BGRA8_EXT. Because we can't rely on that we
		// have stupid workarounds in place on texture creation...
		if (isExtensionPresent("GL_EXT_texture_format_BGRA8888") || isExtensionPresent("GL_APPLE_texture_format_BGRA8888"))
			TextureFormats[ECF_A8R8G8B8] = {GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE};

		// OpenGL ES 3 doesn't include a GL_DEPTH_COMPONENT32, so still use
		// OES_depth_texture for 32-bit depth texture support.
		// OpenGL ES 3 would allow {GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT},
		// but I guess that would have to be called ECF_D32F...
		if (isExtensionPresent("GL_OES_depth_texture"))
			TextureFormats[ECF_D32] = {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT};
	} else {
		// NOTE These are *texture* formats. They may or may not be suitable
		// for render targets. The specs only talks on *sized* formats for the
		// latter but forbids creating textures with sized internal formats,
		// reserving them for renderbuffers.

		static const GLenum HALF_FLOAT_OES = 0x8D61; // not equal to GL_HALF_FLOAT
		TextureFormats[ECF_A1R5G5B5] = {GL_RGBA, GL_RGBA, GL_UNSIGNED_SHORT_5_5_5_1, CColorConverter::convert_A1R5G5B5toR5G5B5A1};
		TextureFormats[ECF_R5G6B5] = {GL_RGB, GL_RGB, GL_UNSIGNED_SHORT_5_6_5};
		TextureFormats[ECF_R8G8B8] = {GL_RGB, GL_RGB, GL_UNSIGNED_BYTE};
		TextureFormats[ECF_A8R8G8B8] = {GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, CColorConverter::convert_A8R8G8B8toA8B8G8R8};

		if (isExtensionPresent("GL_EXT_texture_format_BGRA8888") || isExtensionPresent("GL_APPLE_texture_format_BGRA8888"))
			TextureFormats[ECF_A8R8G8B8] = {GL_BGRA, GL_BGRA, GL_UNSIGNED_BYTE};

		if (isExtensionPresent("GL_OES_texture_half_float")) {
			TextureFormats[ECF_A16B16G16R16F] = {GL_RGBA, GL_RGBA, HALF_FLOAT_OES};
		}
		if (isExtensionPresent("GL_OES_texture_float")) {
			TextureFormats[ECF_A32B32G32R32F] = {GL_RGBA, GL_RGBA, GL_FLOAT};
		}
		if (isExtensionPresent("GL_EXT_texture_rg")) {
			TextureFormats[ECF_R8] = {GL_RED, GL_RED, GL_UNSIGNED_BYTE};
			TextureFormats[ECF_R8G8] = {GL_RG, GL_RG, GL_UNSIGNED_BYTE};

			if (isExtensionPresent("GL_OES_texture_half_float")) {
				TextureFormats[ECF_R16F] = {GL_RED, GL_RED, HALF_FLOAT_OES};
				TextureFormats[ECF_G16R16F] = {GL_RG, GL_RG, HALF_FLOAT_OES};
			}
			if (isExtensionPresent("GL_OES_texture_float")) {
				TextureFormats[ECF_R32F] = {GL_RED, GL_RED, GL_FLOAT};
				TextureFormats[ECF_G32R32F] = {GL_RG, GL_RG, GL_FLOAT};
			}
		}

		if (isExtensionPresent("GL_OES_depth_texture")) {
			TextureFormats[ECF_D16] = {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT};
			// OES_depth_texture includes 32-bit depth texture support.
			TextureFormats[ECF_D32] = {GL_DEPTH_COMPONENT, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT};

			if (isExtensionPresent("GL_OES_packed_depth_stencil"))
				TextureFormats[ECF_D24S8] = {GL_DEPTH_STENCIL, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8};
		}
	}
#endif
}

bool GLSpecificInfo::isVersionAtLeast(int major, int minor) const noexcept
{
	if (GLVersion.Major < major)
		return false;
	if (GLVersion.Major > major)
		return true;
	return GLVersion.Minor >= minor;
}

bool GLSpecificInfo::queryTextureFormat(ECOLOR_FORMAT format) const
{
	return TextureFormats[format].InternalFormat != 0;
}

void GLSpecificInfo::printTextureFormats()
{
	char buf[128];
	for (u32 i = 0; i < static_cast<u32>(ECF_UNKNOWN); i++) {
		auto &info = TextureFormats[i];
		if (!info.InternalFormat) {
			snprintf_irr(buf, sizeof(buf), "%s -> unsupported", ColorFormatNames[i]);
		} else {
			snprintf_irr(buf, sizeof(buf), "%s -> %#06x %#06x %#06x%s",
					ColorFormatNames[i], info.InternalFormat, info.PixelFormat,
					info.PixelType, info.Converter ? " (c)" : "");
		}
		g_irrlogger->log(buf, ELL_DEBUG);
	}
}

GLint GLSpecificInfo::GetInteger(GLenum key)
{
	GLint val = 0;
	glGetIntegerv(key, &val);
	return val;
};

void GLSpecificInfo::ObjectLabel(GLenum identifier, GLuint name, const char *label)
{
	if (Features.KHRDebugSupported) {
		u32 len = static_cast<u32>(strlen(label));
		// Since our texture strings can get quite long we also truncate
		// to a hardcoded limit of 82
		len = std::min(len, std::min(Features.MaxLabelLength, 82U));
		glObjectLabel(identifier, name, len, label);
	}
}

} // end namespace video
