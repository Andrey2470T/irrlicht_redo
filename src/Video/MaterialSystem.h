#pragma once

#include <string>
#include <vector>
#include "EMaterialTypes.h"
#include "EPrimitiveTypes.h"
#include "IReadFile.h"
#include "MaterialRenderer.h"

#include "irrArray.h"
#include "SOverrideMaterial.h"

namespace io
{
class IFileSystem;
}

namespace video
{

class VideoDriver;
class DrawContext;

class MaterialSystem
{
	//! enumeration for rendering modes such as 2d and 3d for minimizing the switching of renderStates.
	enum E_RENDER_MODE
	{
		ERM_NONE = 0, // no render state has been set yet.
		ERM_2D,       // 2d drawing rendermode
		ERM_3D        // 3d rendering mode
	};

	VideoDriver *Driver;
	io::IFileSystem *FileSystem;

	E_RENDER_MODE CurrentRenderMode = ERM_NONE;

	SMaterial Material, LastMaterial;

    core::array<MaterialRenderer *> MaterialRenderers;
	s32 MaterialRenderer2DIdx;

	static u32 BuiltinMaterialsNum;

	SOverrideMaterial OverrideMaterial;
	SMaterial OverrideMaterial2D;
	SMaterial InitMaterial2D;
	bool OverrideMaterial2DEnabled = false;

	//! bool to make all renderstates reset if set to true.
	bool ResetRenderStates = true;
	bool LockRenderStateMode = false;

	bool AllowZWriteOnTransparent = false;

	io::path ShadersPath;
public:
	MaterialSystem(VideoDriver *driver, io::IFileSystem *filesys, const io::path &shadersPath);
	~MaterialSystem();

    MaterialRenderer *getMaterialRenderer(u32 idx) const;
	s32 addMaterialRenderer(MaterialRenderer *renderer);

	SOverrideMaterial &getOverrideMaterial();
	SMaterial &getMaterial2D();
	void enableMaterial2D(bool enable = true);

	void deleteMaterialRenders();

	//! Get current material.
	const SMaterial &getCurrentMaterial() const;

	//! Sets a material.
	void setMaterial(const SMaterial &material);

	//! Adds a new material renderer to the VideoDriver
	s32 addHighLevelShaderMaterial(
            const std::string &vertexShaderProgram,
            const std::string &fragmentShaderProgram,
            const std::string &geometryShaderProgram = "",
            const std::string &shaderName = "",
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack *callback = nullptr,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData = 0);

	s32 addHighLevelShaderMaterial(
                const std::string &vertexShaderProgram,
                const std::string &fragmentShaderProgram = "",
                const std::string &shaderName = "",
				IShaderConstantSetCallBack *callback = nullptr,
				E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
				s32 userData = 0)
	{
		return addHighLevelShaderMaterial(
            vertexShaderProgram, fragmentShaderProgram,
			nullptr, shaderName,
			scene::EPT_TRIANGLES, scene::EPT_TRIANGLE_STRIP, 0,
			callback, baseMaterial, userData);
	}

	s32 addHighLevelShaderMaterialFromFiles(
			const io::path &vertexShaderProgramFileName,
			const io::path &pixelShaderProgramFileName,
			const io::path &geometryShaderProgramFileName,
			const c8 *shaderName = nullptr,
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack *callback = nullptr,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData = 0);

	void deleteShaderMaterial(s32 material);

	void setBasicRenderStates(const SMaterial &material, const SMaterial &lastMaterial, bool resetAllRenderStates);

	//! Compare in SMaterial doesn't check texture parameters, so we should call this on each OnRender call.
	void setTextureRenderStates(const SMaterial &material, bool resetAllRenderstates);

	friend class VideoDriver;
private:
	s32 addHighLevelShaderMaterialFromFiles(
			io::IReadFile *vertexShaderProgram,
			io::IReadFile *pixelShaderProgram = 0,
			io::IReadFile *geometryShaderProgram = 0,
			const c8 *shaderName = nullptr,
			scene::E_PRIMITIVE_TYPE inType = scene::EPT_TRIANGLES,
			scene::E_PRIMITIVE_TYPE outType = scene::EPT_TRIANGLE_STRIP,
			u32 verticesOut = 0,
			IShaderConstantSetCallBack *callback = nullptr,
			E_MATERIAL_TYPE baseMaterial = video::EMT_SOLID,
			s32 userData = 0);

	//! sets the needed renderstates
	void setRenderStates3DMode();

	//! sets the needed renderstates
	void setRenderStates2DMode(bool alpha, bool texture, bool alphaChannel);

	//! Prevent setRenderStateMode calls to do anything.
	// hack to allow drawing meshbuffers in 2D mode.
	// Better solution would be passing this flag through meshbuffers,
	// but the way this is currently implemented in Irrlicht makes this tricky to implement
	void lockRenderStateMode()
	{
		LockRenderStateMode = true;
	}

	//! Allow setRenderStateMode calls to work again
	void unlockRenderStateMode()
	{
		LockRenderStateMode = false;
	}

	void createMaterialRenderers();

	void chooseMaterial2D();

    bool setMaterialTexture(u32 layerIdx, const GLTexture *texture);

	void loadShaderData(const io::path &vertexShaderName, const io::path &fragmentShaderName, c8 **vertexShaderData, c8 **fragmentShaderData);

	bool getWriteZBuffer(const SMaterial &material) const;
};

}
