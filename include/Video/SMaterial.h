// Copyright (C) 2002-2012 Nikolaus Gebhardt
// This file is part of the "Irrlicht Engine".
// For conditions of distribution and use, see copyright notice in irrlicht.h

#pragma once

#include "Image/SColor.h"
#include "Utils/matrix4.h"
#include "Utils/irrMath.h"
#include "Enums/EMaterialTypes.h" // IWYU pragma: export
#include "Enums/EMaterialProps.h" // IWYU pragma: export
#include "Video/SMaterialLayer.h"

namespace video
{
class GLTexture;

//! Flag for MaterialTypeParam (in combination with EMT_ONETEXTURE_BLEND) or for BlendFactor
//! BlendFunc = source * sourceFactor + dest * destFactor
enum E_BLEND_FACTOR : u8
{
	EBF_ZERO = 0,            //!< src & dest	(0, 0, 0, 0)
	EBF_ONE,                 //!< src & dest	(1, 1, 1, 1)
	EBF_DST_COLOR,           //!< src	(destR, destG, destB, destA)
	EBF_ONE_MINUS_DST_COLOR, //!< src	(1-destR, 1-destG, 1-destB, 1-destA)
	EBF_SRC_COLOR,           //!< dest	(srcR, srcG, srcB, srcA)
	EBF_ONE_MINUS_SRC_COLOR, //!< dest	(1-srcR, 1-srcG, 1-srcB, 1-srcA)
	EBF_SRC_ALPHA,           //!< src & dest	(srcA, srcA, srcA, srcA)
	EBF_ONE_MINUS_SRC_ALPHA, //!< src & dest	(1-srcA, 1-srcA, 1-srcA, 1-srcA)
	EBF_DST_ALPHA,           //!< src & dest	(destA, destA, destA, destA)
	EBF_ONE_MINUS_DST_ALPHA, //!< src & dest	(1-destA, 1-destA, 1-destA, 1-destA)
	EBF_SRC_ALPHA_SATURATE,  //!< src	(min(srcA, 1-destA), idem, ...)
	EBF_COUNT
};

//! Values defining the blend operation
enum E_BLEND_OPERATION : u8
{
	EBO_NONE = 0,    //!< No blending happens
	EBO_ADD,         //!< Default blending adds the color values
	EBO_SUBTRACT,    //!< This mode subtracts the color values
	EBO_REVSUBTRACT, //!< This modes subtracts destination from source
	EBO_MIN,         //!< Choose minimum value of each color channel
	EBO_MAX,         //!< Choose maximum value of each color channel
	EBO_MIN_FACTOR,  //!< Choose minimum value of each color channel after applying blend factors, not widely supported
	EBO_MAX_FACTOR,  //!< Choose maximum value of each color channel after applying blend factors, not widely supported
	EBO_MIN_ALPHA,   //!< Choose minimum value of each color channel based on alpha value, not widely supported
	EBO_MAX_ALPHA,   //!< Choose maximum value of each color channel based on alpha value, not widely supported
	EBO_COUNT
};

//! Values defining the blend mode
enum E_BLEND_MODE : u8
{
	EBM_NONE = 0,
	EBM_ALPHA,
	EBM_ADD,
	EBM_SUBTRACT,
	EBM_REVSUBTRACT,
	EBM_MULTIPLY,
	EBM_SCREEN,
	EBM_MIN,
	EBM_MAX
};

//! Comparison function, e.g. for depth buffer test
enum E_COMPARISON_FUNC : u8
{
	//! Depth test disabled (disable also write to depth buffer)
	ECFN_DISABLED = 0,
	//! <= test, default for e.g. depth test
	ECFN_LESSEQUAL = 1,
	//! Exact equality
	ECFN_EQUAL = 2,
	//! exclusive less comparison, i.e. <
	ECFN_LESS,
	//! Succeeds almost always, except for exact equality
	ECFN_NOTEQUAL,
	//! >= test
	ECFN_GREATEREQUAL,
	//! inverse of <=
	ECFN_GREATER,
	//! test succeeds always
	ECFN_ALWAYS,
	//! Test never succeeds
	ECFN_NEVER,
	ECFN_COUNT
};

//! Enum values for enabling/disabling color planes for rendering
enum E_COLOR_PLANE : u8
{
	//! No color enabled
	ECP_NONE = 0,
	//! Alpha enabled
	ECP_ALPHA = 1,
	//! Red enabled
	ECP_RED = 2,
	//! Green enabled
	ECP_GREEN = 4,
	//! Blue enabled
	ECP_BLUE = 8,
	//! All colors, no alpha
	ECP_RGB = 14,
	//! All planes enabled
	ECP_ALL = 15
};

//! Source of the alpha value to take
/** This is currently only supported in EMT_ONETEXTURE_BLEND. You can use an
or'ed combination of values. Alpha values are modulated (multiplied). */
enum E_ALPHA_SOURCE : u8
{
	//! Use no alpha, somewhat redundant with other settings
	EAS_NONE = 0,
	//! Use vertex color alpha
	EAS_VERTEX_COLOR,
	//! Use texture alpha channel
	EAS_TEXTURE
};

//! These flags are used to specify the anti-aliasing and smoothing modes
/** Techniques supported are multisampling, geometry smoothing, and alpha
to coverage.
Some drivers don't support a per-material setting of the anti-aliasing
modes. In those cases, FSAA/multisampling is defined by the device mode
chosen upon creation via SIrrCreationParameters.
*/
enum E_ANTI_ALIASING_MODE : u8
{
	//! Use to turn off anti-aliasing for this material
	EAAM_OFF = 0,
	//! Default anti-aliasing mode
	EAAM_SIMPLE = 1,
	//! High-quality anti-aliasing, not always supported, automatically enables SIMPLE mode
	EAAM_QUALITY = 3,
	//! Enhanced anti-aliasing for transparent materials
	/** Usually used with EMT_TRANSPARENT_ALPHA_CHANNEL_REF and multisampling. */
	EAAM_ALPHA_TO_COVERAGE = 4
};

//! Names for polygon offset direction
const c8 *const PolygonOffsetDirectionNames[] = {
		"Back",
		"Front",
		0,
	};

//! For SMaterial.ZWriteEnable
enum E_ZWRITE : u8
{
	//! zwrite always disabled for this material
	EZW_OFF = 0,

	//! This is the default setting for SMaterial and tries to handle things automatically.
	//! This is what you want to set to enable zwriting.
	//! Usually zwriting is enabled non-transparent materials - as far as Irrlicht can recognize those.
	//! Basically Irrlicht tries to handle the zwriting for you and assumes transparent materials don't need it.
	//! This is addionally affected by IVideoDriver::setAllowZWriteOnTransparent
	EZW_AUTO,

	//! zwrite always enabled for this material
	EZW_ON
};

//! Names for E_ZWRITE
const c8 *const ZWriteNames[] = {
		"Off",
		"Auto",
		"On",
		0,
	};

//! Maximum number of texture an SMaterial can have.
/** SMaterial might ignore some textures in most function, like assignment and comparison,
	when SDLDeviceParameters::MaxTextureUnits is set to a lower number.
*/
constexpr static u32 MATERIAL_MAX_TEXTURES = 8;

//! Struct for holding parameters for a material renderer
// Note for implementors: Serialization is in CNullDriver
class SMaterial
{
public:
	//! Texture layer array.
	SMaterialLayer TextureLayers[MATERIAL_MAX_TEXTURES];

	//! Type of the material. Specifies how everything is blended together
	E_MATERIAL_TYPE MaterialType{EMT_SOLID};

	//! Custom color parameter, can be used by custom shader materials.
	// See MainShaderConstantSetter in Luanti.
	SColor ColorParam{0, 0, 0, 0};

	//! Thickness of non-3dimensional elements such as lines and points.
	f32 Thickness{1.0f};

	E_BLEND_MODE BlendMode{EBM_NONE};

	u8 AlphaSource{EAS_NONE};

	//! A constant z-buffer offset for a polygon/line/point
	/** The range of the value is driver specific.
	On OpenGL you get units which are multiplied by the smallest value that is guaranteed to produce a resolvable offset. */
	f32 PolygonOffsetDepthBias{0.0f};

	//! Variable Z-Buffer offset based on the slope of the polygon.
	/** For polygons looking flat at a camera you could use 0 (for example in a 2D game)
	But in most cases you will have polygons rendered at a certain slope.
	The driver will calculate the slope for you and this value allows to scale that slope.
	The complete polygon offset is: PolygonOffsetSlopeScale*slope + PolygonOffsetDepthBias
	A good default here is to use 1.f if you want to push the polygons away from the camera
	and -1.f to pull them towards the camera.  */
	f32 PolygonOffsetSlopeScale{0.0f};

	//! Is the ZBuffer enabled? Default: ECFN_LESSEQUAL
	/** If you want to disable depth test for this material
	just set this parameter to ECFN_DISABLED. */
	E_COMPARISON_FUNC ZBuffer{ECFN_LESSEQUAL};

	//! Sets the antialiasing mode
	/** Default is EAAM_SIMPLE, i.e. simple multi-sample anti-aliasing. */
	E_ANTI_ALIASING_MODE AntiAliasing{EAAM_SIMPLE};

	//! Defines the enabled color planes
	/** Values are defined as or'ed values of the E_COLOR_PLANE enum.
	Only enabled color planes will be rendered to the current render
	target. Typical use is to disable all colors when rendering only to
	depth or stencil buffer, or using Red and Green for Stereo rendering. */
	E_COLOR_PLANE ColorMask{ECP_ALL};

	//! Draw as wireframe or filled triangles? Default: false
	bool Wireframe{false};

	//! Draw as point cloud or filled triangles? Default: false
	bool PointCloud{false};

	//! Is the zbuffer writable or is it read-only. Default: EZW_AUTO.
	/** If this parameter is not EZW_OFF, you probably also want to set ZBuffer
	to values other than ECFN_DISABLED */
	E_ZWRITE ZWriteEnable{EZW_AUTO};

	//! Is backface culling enabled? Default: true
	bool BackfaceCulling{true};

	//! Is frontface culling enabled? Default: false
	bool FrontfaceCulling{false};

	//! Is fog enabled? Default: false
	bool FogEnable{false};

	//! Shall mipmaps be used if available
	/** Sometimes, disabling mipmap usage can be useful. Default: true */
	bool UseMipMaps{true};

	//! Execute a function on all texture layers.
	/** Useful for setting properties which are not per material, but per
	texture layer, e.g. bilinear filtering. */
	template <typename F>
	void forEachTexture(F &&fn)
	{
		for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; i++) {
			fn(TextureLayers[i]);
		}
	}

	//! Gets the texture transformation matrix for level i
	/** \param i The desired level. Must not be larger than MATERIAL_MAX_TEXTURES
	\return Texture matrix for texture level i. */
	core::matrix4 &getTextureMatrix(u32 i)
	{
		return TextureLayers[i].getTextureMatrix();
	}

	//! Gets the immutable texture transformation matrix for level i
	/** \param i The desired level.
	\return Texture matrix for texture level i, or identity matrix for levels larger than MATERIAL_MAX_TEXTURES. */
	const core::matrix4 &getTextureMatrix(u32 i) const
	{
		if (i < MATERIAL_MAX_TEXTURES)
			return TextureLayers[i].getTextureMatrix();
		else
			return core::IdentityMatrix;
	}

	//! Sets the i-th texture transformation matrix
	/** \param i The desired level.
	\param mat Texture matrix for texture level i. */
	void setTextureMatrix(u32 i, const core::matrix4 &mat)
	{
		if (i >= MATERIAL_MAX_TEXTURES)
			return;
		TextureLayers[i].setTextureMatrix(mat);
	}

	//! Gets the i-th texture
	/** \param i The desired level.
	\return Texture for texture level i, if defined, else 0. */
    GLTexture *getTexture(u32 i) const
	{
		return i < MATERIAL_MAX_TEXTURES ? TextureLayers[i].Texture : nullptr;
	}

	//! Sets the i-th texture
	/** If i>=MATERIAL_MAX_TEXTURES this setting will be ignored.
	\param i The desired level.
	\param tex Texture for texture level i. */
    void setTexture(u32 i, GLTexture *tex)
	{
		if (i >= MATERIAL_MAX_TEXTURES)
			return;
		TextureLayers[i].Texture = tex;
	}

	//! Inequality operator
	/** \param b Material to compare to.
	\return True if the materials differ, else false. */
	inline bool operator!=(const SMaterial &b) const
	{
		bool different =
				MaterialType != b.MaterialType ||
				ColorParam != b.ColorParam ||
				BlendMode != b.BlendMode ||
				AlphaSource != b.AlphaSource ||
				Thickness != b.Thickness ||
				Wireframe != b.Wireframe ||
				PointCloud != b.PointCloud ||
				ZBuffer != b.ZBuffer ||
				ZWriteEnable != b.ZWriteEnable ||
				BackfaceCulling != b.BackfaceCulling ||
				FrontfaceCulling != b.FrontfaceCulling ||
				FogEnable != b.FogEnable ||
				AntiAliasing != b.AntiAliasing ||
				ColorMask != b.ColorMask ||
				PolygonOffsetDepthBias != b.PolygonOffsetDepthBias ||
				PolygonOffsetSlopeScale != b.PolygonOffsetSlopeScale ||
				UseMipMaps != b.UseMipMaps;
		if (different)
			return true;
		for (u32 i = 0; i < MATERIAL_MAX_TEXTURES; ++i) {
			if (TextureLayers[i] != b.TextureLayers[i])
				return true;
		}
		return false;
	}

	//! Equality operator
	/** \param b Material to compare to.
	\return True if the materials are equal, else false. */
	inline bool operator==(const SMaterial &b) const
	{
		return !(b != *this);
	}

	//! Check if material needs alpha blending
	bool isAlphaBlendOperation() const
	{
		return BlendMode == EBM_ALPHA;
	}

	//! Check for some fixed-function transparent types. Still used internally, but might be deprecated soon.
	//! You probably should not use this anymore, IVideoDriver::needsTransparentRenderPass is more useful in most situations
	//! as it asks the material renders directly what they do with the material.
	bool isTransparent() const
	{
		if (MaterialType == EMT_TRANSPARENT_ALPHA_CHANNEL ||
				MaterialType == EMT_TRANSPARENT_VERTEX_ALPHA ||
				MaterialType == EMT_ONETEXTURE_BLEND)
			return true;

		return false;
	}
};

//! global const identity Material
extern const SMaterial IdentityMaterial;

} // end namespace video


template<>
struct std::hash<video::SMaterial>
{
	/// @brief std::hash specialization for video::SMaterial
	std::size_t operator()(const video::SMaterial &m) const noexcept
	{
		std::size_t ret = 0;
		for (auto h : { // the three members most likely to differ
            std::hash<video::GLTexture*>{}(m.getTexture(0)),
			std::hash<int>{}(m.MaterialType),
			std::hash<u32>{}(m.ColorParam.color)
		}) {
			ret += h;
			ret ^= (ret << 6) + (ret >> 2); // distribute bits
		}
		return ret;
	}
};
