// Copyright (C) 2023 Vitaliy Lobachevskiy
// Copyright (C) 2015 Patryk Nadrowski
// Copyright (C) 2009-2010 Amundis
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in Irrlicht.h

#pragma once

#include <unordered_set>

#include "Common.h"
#include "Video/OpenGLFeatures.h"
#include "Image/PixelFormats.h"
#include <string>
#include <array>

namespace video
{

class ExtensionHandler
{
	std::unordered_set<std::string> Extensions;
public:
	ExtensionHandler() = default;

	void initExtensions();

	/// Checks#include "Common.h" whether a named extension is present
	inline bool isExtensionPresent(const std::string &name) const noexcept
	{
		return Extensions.count(name) > 0;;
	}
};

using ColorConverter = void (*)(const void *source, s32 count, void *dest);
struct TextureFormatInfo
{
	GLenum InternalFormat;
	GLenum PixelFormat;
	GLenum PixelType;
	ColorConverter Converter;
};

class GLSpecificInfo : public ExtensionHandler
{
	std::string RendererName;
	std::string VendorName;

	OpenGLFeatures Features;
	OpenGLVersion GLVersion;

public:
	static std::array<TextureFormatInfo, ECF_UNKNOWN> TextureFormats;

	GLSpecificInfo(bool stencilBuffer, bool enableDebug)
	{
		init(stencilBuffer, enableDebug);
	}

	void init(bool stencilBuffer, bool enableDebug);
	void initVersion();
	void initFeatures();
	void initTextureFormats();

	const std::string &getRendererName() const
	{
		return RendererName;
	}
	const std::string &getVendorInfo() const
	{
		return VendorName;
	}
	const OpenGLFeatures &getFeatures() const
	{
		return Features;
	}
	const OpenGLVersion &getOpenGLVersion() const
	{
		return GLVersion;
	}
	bool isVersionAtLeast(int major, int minor = 0) const noexcept;

	bool queryTextureFormat(ECOLOR_FORMAT format) const;
	void printTextureFormats();

	static GLint GetInteger(GLenum key);

	void ObjectLabel(GLenum identifier, GLuint name, const char *label);
};

}
