#pragma once

#include "Common.h"

namespace render
{

// Number of the BlendModes counting from the enum begin
u32 SupportedImageBlendModes = 6;

struct DepthTestState
{
	bool depthTest = false;
	bool depthMask = true;
	CompareFunc depthFunc;
};

struct CullFaceState
{
	bool cullFace = false;
	CullMode cullMode;
};

struct StencilTestState
{
	bool stencilTest = false;
	CompareFunc stencilFunc = CF_ALWAYS;
	u32 stencilMask;
	StencilOp stencilOp;
}

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

	f32 pointSize = 1.0f;
	f32 lineWidth = 1.0f;

	utils::recti viewport;
public:
	//! Constructor
	DrawContext(utils::recti viewportSize)
	{
		setViewportSize(viewportSize);
		
		initContext();
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
	void setStencilFunc(CompareFunc stencilfunc);
	void setStencilMask(u32 stencilmask);
	void setStencilOp(StencilOp stencilop);
	
	void setPointSize(f32 pointsize);
	void setLineWidth(f32 linewidth);
	
	void setViewportSize(utils::recti viewportSize);
private:
	void initContext();
};

}
