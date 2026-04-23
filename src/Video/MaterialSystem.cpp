#include "MaterialSystem.h"

#include "Enums/EVertexAttributes.h"
#include "Video/IShaderConstantSetCallBack.h"
#include "Video/VideoDriver.h"
#include "Device/Logger.h"
#include "IO/IFileSystem.h"

#include "Common.h"
#include "Video/VideoDriver.h"
#include "Video/DrawContext.h"
#include "Video/RenderTarget.h"
#include "Video/MaterialRenderer.h"
#include "MaterialCallbacks.h"
#include "Video/Texture.h"

namespace video
{

u32 MaterialSystem::BuiltinMaterialsNum = 6;

MaterialSystem::MaterialSystem(VideoDriver *driver, io::IFileSystem *filesys, const io::path &shadersPath)
	: Driver(driver), FileSystem(filesys), ShadersPath(shadersPath)
{
	InitMaterial2D.AntiAliasing = EAAM_OFF;
	InitMaterial2D.ZWriteEnable = EZW_OFF;
	InitMaterial2D.ZBuffer = ECFN_DISABLED;
	InitMaterial2D.UseMipMaps = false;
	InitMaterial2D.forEachTexture([](auto &tex) {
		tex.MinFilter = ETMINF_NEAREST_MIPMAP_NEAREST;
		tex.MagFilter = ETMAGF_NEAREST;
		tex.TextureWrapU = ETC_REPEAT;
		tex.TextureWrapV = ETC_REPEAT;
		tex.TextureWrapW = ETC_REPEAT;
	});
	OverrideMaterial2D = InitMaterial2D;
}

MaterialSystem::~MaterialSystem()
{
	deleteMaterialRenders();
}

MaterialRenderer *MaterialSystem::getMaterialRenderer(u32 idx) const
{
	if (idx < MaterialRenderers.size())
		return MaterialRenderers[idx];
	else
		return nullptr;
}

s32 MaterialSystem::addMaterialRenderer(MaterialRenderer *renderer)
{
	if (!renderer)
		return -1;

    MaterialRenderer *r = renderer;
	r->grab();
	MaterialRenderers.push_back(r);
	return MaterialRenderers.size() - 1;
}

SOverrideMaterial &MaterialSystem::getOverrideMaterial()
{
	return OverrideMaterial;
}

SMaterial &MaterialSystem::getMaterial2D()
{
	return OverrideMaterial2D;
}

void MaterialSystem::enableMaterial2D(bool enable)
{
	OverrideMaterial2DEnabled = enable;
}

//! deletes all material renderers
void MaterialSystem::deleteMaterialRenders()
{
	// delete material renderers
	for (u32 i = 0; i < MaterialRenderers.size(); ++i)
		if (MaterialRenderers[i])
			MaterialRenderers[i]->drop();

	MaterialRenderers.clear();
}

const SMaterial &MaterialSystem::getCurrentMaterial() const
{
	return Material;
}

// Same as COpenGLDriver::TextureFlipMatrix
static const core::matrix4 s_texture_flip_matrix = {
	1,  0, 0, 0,
	0, -1, 0, 0,
	0,  1, 1, 0,
	0,  0, 0, 1
};

//! Sets a material.
void MaterialSystem::setMaterial(const SMaterial &material)
{
	Material = material;
	OverrideMaterial.apply(Material);

	auto features = Driver->getFeatures();
	for (u32 i = 0; i < features.MaxTextureUnits; ++i) {
		auto *texture = material.getTexture(i);
		Driver->Context->setTextureUnit(i, texture);
		if (texture) {
			Driver->setTransform((E_TRANSFORMATION_STATE)(ETS_TEXTURE_0 + i),
				texture->isRenderTarget()
					? material.getTextureMatrix(i) * s_texture_flip_matrix
					: material.getTextureMatrix(i)
			);
		}
	}
}

//! Adds a new material renderer to the VideoDriver, using GLSL to render geometry.
s32 MaterialSystem::addHighLevelShaderMaterial(const std::string &vertexShaderProgram,
    const std::string &fragmentShaderProgram,
    const std::string &geometryShaderProgram,
    const std::string &shaderName,
    scene::E_PRIMITIVE_TYPE inType,
    scene::E_PRIMITIVE_TYPE outType,
    u32 verticesOut,
    IShaderConstantSetCallBack *callback,
    E_MATERIAL_TYPE baseMaterial,
    s32 userData)
{
	s32 nr = -1;
	MaterialRenderer *r = new MaterialRenderer(
			Driver, nr, vertexShaderProgram,
            fragmentShaderProgram, shaderName,
			callback, baseMaterial, userData);

	r->drop();
	return nr;
}

s32 MaterialSystem::addHighLevelShaderMaterialFromFiles(
	const io::path &vertexShaderProgramFileName,
	const io::path &pixelShaderProgramFileName,
	const io::path &geometryShaderProgramFileName,
	const std::string &shaderName,
	scene::E_PRIMITIVE_TYPE inType, scene::E_PRIMITIVE_TYPE outType,
	u32 verticesOut,
	IShaderConstantSetCallBack *callback,
	E_MATERIAL_TYPE baseMaterial,
	s32 userData)
{
	std::string vs = "";
	std::string ps = "";
	std::string gs = "";

	if (vertexShaderProgramFileName.size()) {
		auto vsfile = FileSystem->createAndOpenFile(vertexShaderProgramFileName);
		if (!vsfile) {
			g_irrlogger->log("Could not open vertex shader program file",
					vertexShaderProgramFileName, ELL_WARNING);
		}
		else {
			const long size = vsfile->getSize();
			if (size) {
				vs.resize(size);
				vsfile->read((void *)vs.c_str(), size);
			}
			vsfile->drop();
		}
	}

	if (pixelShaderProgramFileName.size()) {
		auto psfile = FileSystem->createAndOpenFile(pixelShaderProgramFileName);
		if (!psfile) {
			g_irrlogger->log("Could not open pixel shader program file",
					pixelShaderProgramFileName, ELL_WARNING);
		}
		else {
			const long size = psfile->getSize();
			if (size) {
				ps.resize(size);
				psfile->read((void *)ps.c_str(), size);
			}
			psfile->drop();
		}
	}

	if (geometryShaderProgramFileName.size()) {
		auto gsfile = FileSystem->createAndOpenFile(geometryShaderProgramFileName);
		if (!gsfile) {
			g_irrlogger->log("Could not open geometry shader program file",
					geometryShaderProgramFileName, ELL_WARNING);
		}
		else {
			const long size = gsfile->getSize();
			if (size) {
				gs.resize(size);
				gsfile->read((void *)gs.c_str(), size);
			}
			gsfile->drop();
		}
	}

	s32 result = addHighLevelShaderMaterial(
			vs, ps, gs, shaderName,
			inType, outType, verticesOut,
			callback, baseMaterial, userData);

	return result;
}

void MaterialSystem::deleteShaderMaterial(s32 material)
{
	// Only custom materials are allowed to be deleted
	const u32 idx = (u32)material;
	if (idx < MaterialSystem::BuiltinMaterialsNum || idx >= MaterialRenderers.size())
		return;

	// if this is the last material we can drop it without consequence
	if (idx == MaterialRenderers.size() - 1) {
		if (MaterialRenderers[idx])
			MaterialRenderers[idx]->drop();
		MaterialRenderers.erase(idx);
		return;
	}

	if (MaterialRenderers[idx]) {
		MaterialRenderers[idx]->drop();
		MaterialRenderers[idx] = nullptr;
	}
}

void MaterialSystem::setRenderStates3DMode()
{
	if (LockRenderStateMode)
		return;

	if (CurrentRenderMode != ERM_3D) {
		// Reset Texture Stages
		Driver->Context->enableBlend(false);
		Driver->Context->setBlendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA);

		ResetRenderStates = true;
	}

	if (ResetRenderStates || LastMaterial != Material) {
		// set new material.
		if (static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
			MaterialRenderers[Material.MaterialType]->OnSetMaterial(
					Material, LastMaterial, ResetRenderStates);

		LastMaterial = Material;

		ResetRenderStates = false;
	}

	if (static_cast<u32>(Material.MaterialType) < MaterialRenderers.size())
		MaterialRenderers[Material.MaterialType]->OnRender(scene::EVT_3D);

	CurrentRenderMode = ERM_3D;
}

//! Can be called by an MaterialRenderer to make its work easier.
void MaterialSystem::setBasicRenderStates(const SMaterial &material, const SMaterial &lastmaterial, bool resetAllRenderStates)
{
	// ZBuffer
	switch (material.ZBuffer) {
	case ECFN_DISABLED:
		Driver->Context->enableDepthTest(false);
		break;
	case ECFN_COUNT:
		break;
	default:
		Driver->Context->enableDepthTest(true);
		Driver->Context->setDepthFunc(material.ZBuffer);
		break;
	}

	// ZWrite
	Driver->Context->setDepthMask(getWriteZBuffer(material));

	// Face culling
	Driver->Context->enableCullFace(material.FrontfaceCulling || material.BackfaceCulling);

	if ((material.FrontfaceCulling) && (material.BackfaceCulling))
		Driver->Context->setCullMode(ECM_FRONT_AND_BACK);
	else if (material.BackfaceCulling)
		Driver->Context->setCullMode(ECM_BACK);
	else if (material.FrontfaceCulling)
		Driver->Context->setCullMode(ECM_FRONT);

	// Color Mask
	Driver->Context->setColorMask(material.ColorMask);

	auto features = Driver->getFeatures();

	// Blend Equation
	if (material.BlendOperation == EBO_NONE)
		Driver->Context->enableBlend(false);
	else {
		Driver->Context->enableBlend(true);

		switch (material.BlendOperation) {
		case EBO_ADD:
		case EBO_SUBTRACT:
		case EBO_REVSUBTRACT:
			Driver->Context->setBlendOp(material.BlendOperation);
			break;
		case EBO_MIN:
			if (features.BlendMinMaxSupported)
				Driver->Context->setBlendOp(material.BlendOperation);
			else
				g_irrlogger->log("Attempt to use EBO_MIN without driver support", ELL_WARNING);
			break;
		case EBO_MAX:
			if (features.BlendMinMaxSupported)
				Driver->Context->setBlendOp(material.BlendOperation);
			else
				g_irrlogger->log("Attempt to use EBO_MAX without driver support", ELL_WARNING);
			break;
		default:
			break;
		}
	}

	// Blend Factor
	if (IR(material.BlendFactor) & 0xFFFFFFFF // TODO: why the & 0xFFFFFFFF?
			&& material.MaterialType != EMT_ONETEXTURE_BLEND) {
		E_BLEND_FACTOR srcRGBFact = EBF_ZERO;
		E_BLEND_FACTOR dstRGBFact = EBF_ZERO;
		E_BLEND_FACTOR srcAlphaFact = EBF_ZERO;
		E_BLEND_FACTOR dstAlphaFact = EBF_ZERO;
		E_MODULATE_FUNC modulo = EMFN_MODULATE_1X;
		u32 alphaSource = 0;

		unpack_textureBlendFuncSeparate(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact, modulo, alphaSource, material.BlendFactor);

		Driver->Context->setBlendSeparateFunc(srcRGBFact, dstRGBFact, srcAlphaFact, dstAlphaFact);
	}

	// fillmode
	if (Driver->getVersion().Spec != OpenGLSpec::ES && // not supported in gles
			(resetAllRenderStates ||
			lastmaterial.Wireframe != material.Wireframe ||
			lastmaterial.PointCloud != material.PointCloud)) {
		Driver->Context->setPolygonMode(ECM_FRONT_AND_BACK,
			material.Wireframe ? EPM_LINE :
			material.PointCloud ? EPM_POINT :
			EPM_FILL);
	}

	// Polygon Offset
	if (resetAllRenderStates ||
			lastmaterial.PolygonOffsetDepthBias != material.PolygonOffsetDepthBias ||
			lastmaterial.PolygonOffsetSlopeScale != material.PolygonOffsetSlopeScale) {
		bool polygonOffset = material.PolygonOffsetDepthBias || material.PolygonOffsetSlopeScale;
		Driver->Context->enablePolygonOffset(polygonOffset);

		if (polygonOffset)
			Driver->Context->setPolygonOffsetParams(material.PolygonOffsetSlopeScale, material.PolygonOffsetDepthBias);
	}

	if (resetAllRenderStates || lastmaterial.Thickness != material.Thickness)
		Driver->Context->setLineWidth(core::clamp(material.Thickness, features.DimAliasedLine[0], features.DimAliasedLine[1]));

	// Anti aliasing
	// Deal with MSAA even if it's not enabled in the OpenGL context, we might be
	// rendering to an FBO with multisampling.
	if (resetAllRenderStates || lastmaterial.AntiAliasing != material.AntiAliasing) {
		if (material.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
			Driver->Context->enableSampleCoverage(true);
		else if (lastmaterial.AntiAliasing & EAAM_ALPHA_TO_COVERAGE)
			Driver->Context->enableSampleCoverage(false);
	}

	// Texture parameters
	setTextureRenderStates(material, resetAllRenderStates);
}

//! Compare in SMaterial doesn't check texture parameters, so we should call this on each OnRender call.
void MaterialSystem::setTextureRenderStates(const SMaterial &material, bool resetAllRenderstates)
{
    auto features = Driver->getFeatures();

    // Set textures to TU/TIU and apply filters to them

    for (s32 i = features.MaxTextureUnits - 1; i >= 0; --i) {
        auto tex = const_cast<GLTexture *>(Driver->Context->getTextureUnit(i));

        if (!tex)
            continue;

        Driver->Context->activateUnit(i);

        const auto &layer = material.TextureLayers[i];

        TextureSettings newSettings;
        newSettings.HasMipMaps = material.UseMipMaps;
        newSettings.WrapU = (video::E_TEXTURE_CLAMP)layer.TextureWrapU;
        newSettings.WrapV = (video::E_TEXTURE_CLAMP)layer.TextureWrapV;
        newSettings.MinF = layer.MinFilter;
        newSettings.MagF = layer.MagFilter;
        newSettings.LodBias = layer.LODBias;
        newSettings.AnisotropyFilter = layer.AnisotropicFilter;

        tex->updateParameters(newSettings, resetAllRenderstates);
    }
}

//! sets the needed renderstates
void MaterialSystem::setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel)
{
	if (LockRenderStateMode)
		return;

	CurrentRenderMode = ERM_2D;

	MaterialRenderers[MaterialRenderer2DIdx]->OnSetMaterial(Material, LastMaterial, true);
	LastMaterial = Material;

	// no alphaChannel without texture
	alphaChannel &= texture;

	bool alpha_blend = alphaChannel || alpha;
	Driver->Context->enableBlend(alpha_blend);

	if (alpha_blend) {
		Driver->Context->setBlendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA);
		Driver->Context->setBlendOp(EBO_ADD);
	}

	Material.setTexture(0, const_cast<GLTexture *>(Driver->Context->getTextureUnit(0)));
	Driver->setTransform(ETS_TEXTURE_0, core::IdentityMatrix);

	if (texture) {
		if (OverrideMaterial2DEnabled)
			setTextureRenderStates(OverrideMaterial2D, false);
		else
			setTextureRenderStates(InitMaterial2D, false);
	}

	MaterialRenderers[MaterialRenderer2DIdx]->OnRender(scene::EVT_3D);
}

void MaterialSystem::chooseMaterial2D()
{
	if (!OverrideMaterial2DEnabled)
		Material = InitMaterial2D;

	if (OverrideMaterial2DEnabled) {
		OverrideMaterial2D.ZWriteEnable = EZW_OFF;
		OverrideMaterial2D.ZBuffer = ECFN_DISABLED; // it will be ECFN_DISABLED after merge

		Material = OverrideMaterial2D;
	}
}

void MaterialSystem::createMaterialRenderers()
{
	// Create callbacks.

	MaterialSolidCB *SolidCB = new MaterialSolidCB();
	MaterialSolidCB *TransparentAlphaChannelCB = new MaterialSolidCB();
	MaterialSolidCB *TransparentAlphaChannelRefCB = new MaterialSolidCB();
	MaterialSolidCB *TransparentVertexAlphaCB = new MaterialSolidCB();
	MaterialOneTextureBlendCB *OneTextureBlendCB = new MaterialOneTextureBlendCB();
	Material2DCB *Renderer2DCB = new Material2DCB();

	// Create built-in materials.
	// The addition order must be the same as in the E_MATERIAL_TYPE enumeration. Thus the

	core::stringc VertexShader = ShadersPath + "Solid.vsh";

	// EMT_SOLID
	core::stringc FragmentShader = ShadersPath + "Solid.fsh";
	addHighLevelShaderMaterialFromFiles(VertexShader, FragmentShader, "", "Solid",
			scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0, SolidCB, EMT_SOLID, 0);

	// EMT_TRANSPARENT_ALPHA_CHANNEL
	FragmentShader = ShadersPath + "TransparentAlphaChannel.fsh";
	addHighLevelShaderMaterialFromFiles(VertexShader, FragmentShader, "", "TransparentAlphaChannel",
			scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0, TransparentAlphaChannelCB, EMT_TRANSPARENT_ALPHA_CHANNEL, 0);

	// EMT_TRANSPARENT_ALPHA_CHANNEL_REF
	FragmentShader = ShadersPath + "TransparentAlphaChannelRef.fsh";
	addHighLevelShaderMaterialFromFiles(VertexShader, FragmentShader, "", "TransparentAlphaChannelRef",
			scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0, TransparentAlphaChannelRefCB, EMT_SOLID, 0);

	// EMT_TRANSPARENT_VERTEX_ALPHA
	FragmentShader = ShadersPath + "TransparentVertexAlpha.fsh";
	addHighLevelShaderMaterialFromFiles(VertexShader, FragmentShader, "", "TransparentVertexAlpha",
			scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0, TransparentVertexAlphaCB, EMT_TRANSPARENT_ALPHA_CHANNEL, 0);

	// EMT_ONETEXTURE_BLEND
	FragmentShader = ShadersPath + "OneTextureBlend.fsh";
	addHighLevelShaderMaterialFromFiles(VertexShader, FragmentShader, "", "OneTextureBlend",
			scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0, OneTextureBlendCB, EMT_ONETEXTURE_BLEND, 0);

	// EMT_2D
	VertexShader = ShadersPath + "Renderer2D.vsh";
	FragmentShader = ShadersPath + "Renderer2D.fsh";
	MaterialRenderer2DIdx = addHighLevelShaderMaterialFromFiles(VertexShader, FragmentShader, "", "Renderer2D",
			scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0, Renderer2DCB, EMT_SOLID, 0);

	// Drop callbacks.

	SolidCB->drop();
	TransparentAlphaChannelCB->drop();
	TransparentAlphaChannelRefCB->drop();
	TransparentVertexAlphaCB->drop();
	OneTextureBlendCB->drop();
	Renderer2DCB->drop();
}

bool MaterialSystem::setMaterialTexture(u32 layerIdx, const GLTexture *texture)
{
    Material.TextureLayers[layerIdx].Texture = const_cast<GLTexture *>(texture); // function uses const-pointer for texture because all draw functions use const-pointers already
	return Driver->Context->setTextureUnit(0, texture);
}

void MaterialSystem::loadShaderData(const io::path &vertexShaderName, const io::path &fragmentShaderName, c8 **vertexShaderData, c8 **fragmentShaderData)
{
	io::path vsPath(ShadersPath);
	vsPath += vertexShaderName;

	io::path fsPath(ShadersPath);
	fsPath += fragmentShaderName;

	*vertexShaderData = 0;
	*fragmentShaderData = 0;

	io::IReadFile *vsFile = FileSystem->createAndOpenFile(vsPath);
	if (!vsFile) {
		std::string warning("Warning: Missing shader files needed to simulate fixed function materials:\n");
		warning.append(vsPath.c_str()).append("\n");
		warning += "Shaderpath can be changed in SIrrCreationParamters::OGLES2ShaderPath";
		g_irrlogger->log(warning.c_str(), ELL_WARNING);
		return;
	}

	io::IReadFile *fsFile = FileSystem->createAndOpenFile(fsPath);
	if (!fsFile) {
		std::string warning("Warning: Missing shader files needed to simulate fixed function materials:\n");
		warning.append(fsPath.c_str()).append("\n");
		warning += "Shaderpath can be changed in SIrrCreationParamters::OGLES2ShaderPath";
		g_irrlogger->log(warning.c_str(), ELL_WARNING);
		return;
	}

	long size = vsFile->getSize();
	if (size) {
		*vertexShaderData = new c8[size + 1];
		vsFile->read(*vertexShaderData, size);
		(*vertexShaderData)[size] = 0;
	}
	{
		auto tmp = std::string("Loaded ") + std::to_string(size) + " bytes for vertex shader " + vertexShaderName.c_str();
		g_irrlogger->log(tmp.c_str(), ELL_INFORMATION);
	}

	size = fsFile->getSize();
	if (size) {
		// if both handles are the same we must reset the file
		if (fsFile == vsFile)
			fsFile->seek(0);

		*fragmentShaderData = new c8[size + 1];
		fsFile->read(*fragmentShaderData, size);
		(*fragmentShaderData)[size] = 0;
	}
	{
		auto tmp = std::string("Loaded ") + std::to_string(size) + " bytes for fragment shader " + fragmentShaderName.c_str();
		g_irrlogger->log(tmp.c_str(), ELL_INFORMATION);
	}

	vsFile->drop();
	fsFile->drop();
}

bool MaterialSystem::getWriteZBuffer(const SMaterial &material) const
{
	switch (material.ZWriteEnable) {
	case video::EZW_OFF:
		return false;
	case video::EZW_AUTO:
		return AllowZWriteOnTransparent || !Driver->needsTransparentRenderPass(material);
	case video::EZW_ON:
		return true;
	}
	return true;
}

}
