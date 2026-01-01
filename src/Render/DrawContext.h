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
    NONE
};

struct BlendState
{
    bool enabled = false;
    GLBlendMode mode = GLBlendMode::NONE;
    img::colorf color;
    BlendFunc func_srcrgb = BF_COUNT;
    BlendFunc func_srca = BF_COUNT;
    BlendFunc func_destrgb = BF_COUNT;
    BlendFunc func_desta = BF_COUNT;
    BlendOp op = BO_COUNT;
};

struct DepthTestState
{
	bool enabled = false;
	bool mask = false;
	CompareFunc func = CF_COUNT;
};

struct CullFaceState
{
	bool enabled = false;
	CullMode mode = CM_COUNT;
};

struct StencilTestState
{
	bool enabled = false;
	CompareFunc func = CF_COUNT;
	s32 ref;
	u32 mask;
	StencilOp sfail_op = SO_COUNT;
	StencilOp dpfail_op = SO_COUNT;
	StencilOp dppass_op = SO_COUNT;
};

struct ScissorTestState
{
	bool enabled = false;
	recti box;
};

struct PolygonOffsetState
{
    bool enabled = false;
    f32 slope_scale;
    f32 depth_bias;
};

struct PolygonModeState
{
    CullMode face = CM_COUNT;
    PolygonMode mode = PM_COUNT;
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
	FrameBuffer *curFBO = nullptr;
	Shader *curShader = nullptr;
	Mesh *curMesh = nullptr;
	UniformBuffer *curUBO = nullptr;

    u32 maxTextureUnits;
    std::vector<Texture *> textureUnits;
    u32 activeUnit;

    BlendState curBlend;
	DepthTestState curDepthTest;

	CullFaceState curCullFace;

	StencilTestState curStencilTest;
	
	ScissorTestState curScissorTest;

    PolygonOffsetState curPolygonOffset;

    PolygonModeState curPolygonMode;

    f32 pointSize;
    f32 lineWidth;

    bool sampleCoverage = false;

	recti viewport;

    u8 colorMask;
public:
	//! Constructor
    DrawContext(const recti &viewportSize, u32 _maxTextureUnits)
		: maxTextureUnits(_maxTextureUnits)
	{
        textureUnits.resize(_maxTextureUnits, nullptr);

		initContext(viewportSize);
	}

	//! Getters
    recti getViewportSize() const
	{
		return viewport;
	}

	FrameBuffer *getFrameBuffer() const;
	Shader *getShader() const;
	Mesh *getMesh() const;
	UniformBuffer *getUniformBuffer() const;
    Texture *getTextureUnit(u32 index) const;
    std::vector<Texture *> getTextureUnits() const;

    BlendState getBlendState() const;
	DepthTestState getDepthTest() const;
	CullFaceState getCullFace() const;
	StencilTestState getStencilTest() const;
	ScissorTestState getScissorTest() const;
    PolygonOffsetState getPolygonOffset() const;
    PolygonModeState getPolygonMode() const;

    u8 getColorMask() const;

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

    void setViewportSize(recti viewportSize);

    void clearBuffers(u16 flags, img::color8 color=img::black, f32 depth=1.0f, u8 stencil=0);

    void setColorMask(u8 mask);
private:
    void initContext(recti viewportSize);
};

}
