#pragma once

#include "Image/BlendModes.h"
#include "FrameBuffer.h"
#include "Shader.h"
#include "Mesh.h"
#include "UniformBuffer.h"
#include "Texture.h"

namespace render
{

enum class GLBlendMode : u8
{
    NORMAL,
    ALPHA,
    ADD,
    SUBTRACT,
    CUSTOM
};

struct BlendState
{
    bool enabled = true;
    GLBlendMode mode = GLBlendMode::NORMAL;
    img::colorf color;
    BlendFunc func_srcrgb = BF_SRC_ALPHA;
    BlendFunc func_srca = BF_SRC_ALPHA;
    BlendFunc func_destrgb = BF_ONE_MINUS_SRC_ALPHA;
    BlendFunc func_desta = BF_ONE_MINUS_SRC_ALPHA;
    BlendOp op = BO_ADD;
};

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

struct PolygonOffsetState
{
    bool enabled = false;
    f32 slope_scale;
    f32 depth_bias;
};

struct PolygonModeState
{
    CullMode face;
    PolygonMode mode;
};

enum ClearBufferFlags : u8
{
    CBF_NONE,
    CBF_COLOR,
    CBF_DEPTH,
    CBF_STENCIL,
};

enum ColorPlane : u8
{
    //! No color enabled
    CP_NONE = 0,
    //! Alpha enabled
    CP_ALPHA = 1,
    //! Red enabled
    CP_RED = 2,
    //! Green enabled
    CP_GREEN = 4,
    //! Blue enabled
    CP_BLUE = 8,
    //! All colors, no alpha
    CP_RGB = 14,
    //! All planes enabled
    CP_ALL = 15
};

class DrawContext
{
	FrameBuffer *curFBO;
	Shader *curShader;
	Mesh *curMesh;
	UniformBuffer *curUBO;

    u32 maxTextureUnits;
	std::vector<Texture *> activeUnits;

    BlendState curBlend;
	DepthTestState curDepthTest;

	CullFaceState curCullFace;

	StencilTestState curStencilTest;
	
	ScissorTestState curScissorTest;

    PolygonOffsetState curPolygonOffset;

    PolygonModeState curPolygonMode;

	f32 pointSize = 1.0f;
	f32 lineWidth = 1.0f;

    bool sampleCoverage;

	utils::recti viewport;

    u8 colorMask;
public:
	//! Constructor
    DrawContext(const utils::recti &viewportSize, u32 _maxTextureUnits)
		: maxTextureUnits(_maxTextureUnits)
	{
		activeUnits.resize(_maxTextureUnits, nullptr);

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

    BlendState getBlendState() const;
	DepthTestState getDepthTest() const;
	CullFaceState getCullFace() const;
	StencilTestState getStencilTest() const;
	ScissorTestState getScissorTest() const;
    PolygonOffsetState getPolygonOffset() const;
    PolygonMode getPolygonMode() const;

	//! Setters
	void setFrameBuffer(FrameBuffer *fbo);
	void setShader(Shader *shader);
	void setMesh(Mesh *mesh);
	void setUniformBuffer(UniformBuffer *ubo);
	void setActiveUnit(u32 index, Texture *texture);

    void enableBlend(bool blend);
    void setBlendMode(GLBlendMode mode);
    void setBlendColor(const img::colorf &color);
    void setBlendFunc(BlendFunc srcfunc, BlendFunc destfunc);
    void setBlendSeparateFunc(BlendFunc srcrgb_func, BlendFunc destrgb_func,
        BlendFunc srca_func, BlendFunc desta_func);
    void setBlendOp(BlendOp op);

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
    void setScissorBox(const recti &box);

    void enablePolygonOffset(bool polygonoffset);
    void setPolygonOffsetParams(f32 slope_scaled, f32 depth_bias);

    void setPolygonMode(CullMode face, PolygonMode mode);

	void setPointSize(f32 pointsize);
	void setLineWidth(f32 linewidth);

    void enableSampleCoverage(bool samplecoverage);

	void setViewportSize(utils::recti viewportSize);

    void clearBuffers(u16 flags, img::color8 color=img::color8(), f32 depth=0.0f, u8 stencil=0);

    void setColorMask(u8 mask);
private:
	void initContext(utils::recti viewportSize);
};

}
