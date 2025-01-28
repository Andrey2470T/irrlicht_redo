#pragma once

#include "Image/BlendModes.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Mesh.h"
#include "UniformBuffer.h"
#include "Texture.h"

namespace render
{

struct DepthTestState
{
	bool enabled = false;
	bool mask = true;
	CompareFunc func;
};

struct CullFaceState
{
	bool enabled = false;
	CullMode mode;
};

struct StencilTestState
{
	bool enabled = false;
	CompareFunc func = CF_ALWAYS;
	s32 ref;
	u32 mask;
	StencilOp sfail_op;
	StencilOp dpfail_op;
	StencilOp dppass_op;
};

struct ScissorTestState
{
	bool enabled = false;
	utils::recti box;
};

class DrawContext
{
	FrameBuffer *curFBO;
	Shader *curShader;
	Mesh *curMesh;
	UniformBuffer *curUBO;

	std::vector<Texture *> activeUnits;

	img::BlendMode curMode;
	DepthTestState curDepthTest;

	CullFaceState curCullFace;

	StencilTestState curStencilTest;
	
	ScissorTestState curScissorTest;

	f32 pointSize = 1.0f;
	f32 lineWidth = 1.0f;

	utils::recti viewport;
public:
	//! Constructor
	DrawContext(utils::recti viewportSize, u32 maxTextureUnits)
	{
		activeUnits.resize(maxTextureUnits, nullptr);

		initContext(viewportSize);
	}

	//! Getters
	utils::recti getViewportSize() const
	{
		return viewport;
	}

	FrameBuffer *getFrameBuffer() const;
	Shader *getShader() const;
	Mesh *getMesh() const;
	UniformBuffer *getUniformBuffer() const;
	Texture *getActiveUnit(u32 index) const;
	std::vector<Texture *> getActiveUnits() const;

	img::BlendMode getBlendMode() const;
	DepthTestState getDepthTest() const;
	CullFaceState getCullFace() const;
	StencilTestState getStencilTest() const;
	ScissorTestState getScissorTest() const;

	//! Setters
	void setFrameBuffer(FrameBuffer *fbo);
	void setShader(Shader *shader);
	void setMesh(Mesh *mesh);
	void setUniformBuffer(UniformBuffer *ubo);
	void setActiveUnit(u32 index, Texture *texture);

	void setBlendMode(img::BlendMode blendmode);

	void enableDepthTest(bool depthtest);
	void setDepthMask(bool depthmask);
	void setDepthFunc(CompareFunc depthfunc);

	void enableCullFace(bool cullface);
	void setCullMode(CullMode cullmode);

	void enableStencilTest(bool stenciltest);
	void setStencilFunc(CompareFunc stencilfunc, s32 ref, u32 mask);
	void setStencilMask(u32 stencilmask);
	void setStencilOp(StencilOp _sfail_op, StencilOp _dpfail_op, StencilOp _dppass_op);
	
	void enableScissorTest(bool scissortest);
	void setScissorBox(utils::recti box);

	void setPointSize(f32 pointsize);
	void setLineWidth(f32 linewidth);

	void setViewportSize(utils::recti viewportSize);
private:
	void initContext(utils::recti viewportSize);
};

}
