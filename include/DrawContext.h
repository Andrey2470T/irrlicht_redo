#pragma once

#include "SMaterial.h"
#include "ITexture.h"
#include <vector>

namespace video
{

enum E_CULL_MODE : u8
{
	ECM_BACK = 0,
	ECM_FRONT,
	ECM_FRONT_AND_BACK,
	ECM_COUNT
};

enum E_STENCIL_OP : u8
{
	ESO_KEEP = 0,
	ESO_ZERO,
	ESO_REPLACE,
	ESO_INVERT,
	ESO_COUNT
};

enum E_POLYGON_MODE : u8
{
	EPM_FILL = 0,
	EPM_POINT,
	EPM_LINE,
	EPM_COUNT
};

struct BlendState
{
	bool enabled = false;
	E_BLEND_OPERATION mode = EBO_ADD;
	video::SColorf color;
	E_BLEND_FACTOR func_srcrgb = EBF_ZERO;
	E_BLEND_FACTOR func_srca = EBF_ZERO;
	E_BLEND_FACTOR func_destrgb = EBF_ONE;
	E_BLEND_FACTOR func_desta = EBF_ONE;
};

struct DepthTestState
{
	bool enabled = false;
	bool mask = true;
	E_COMPARISON_FUNC func = ECFN_LESS;
};

struct CullFaceState
{
	bool enabled = false;
	E_CULL_MODE mode = ECM_BACK;
};

struct StencilTestState
{
	bool enabled = false;
	E_COMPARISON_FUNC func = ECFN_LESS;
	s32 ref;
	u32 mask;
	E_STENCIL_OP sfail_op = ESO_KEEP;
	E_STENCIL_OP dpfail_op = ESO_KEEP;
	E_STENCIL_OP dppass_op = ESO_KEEP;
};

struct ScissorTestState
{
	bool enabled = false;
	core::recti box;
};

struct PolygonOffsetState
{
    bool enabled = false;
    f32 slope_scale;
    f32 depth_bias;
};

struct PolygonModeState
{
    E_CULL_MODE face = ECM_BACK;
    E_POLYGON_MODE mode = EPM_FILL;
};

enum ClearBufferFlags : u8
{
    CBF_NONE,
    CBF_COLOR,
    CBF_DEPTH,
    CBF_STENCIL,
};

class RenderTarget;
class VideoDriver;

class DrawContext
{
	VideoDriver *driver;
	RenderTarget *curRT = nullptr;
	u32 curProgramID = 0;

    u32 maxTextureUnits;
	std::vector<const ITexture *> textureUnits;
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

	core::recti viewport;

    u8 colorMask;
public:
	//! Constructor
	DrawContext(VideoDriver *_driver);
	~DrawContext();

	//! Getters
    core::recti getViewportSize() const
	{
		return viewport;
	}

	RenderTarget *getRenderTarget() const;
	u32 getProgram() const;
	const ITexture *getTextureUnit(u32 index) const;
	std::vector<const ITexture *> getTextureUnits() const;

	BlendState getBlendState() const;
	DepthTestState getDepthTest() const;
	CullFaceState getCullFace() const;
	StencilTestState getStencilTest() const;
	ScissorTestState getScissorTest() const;
	PolygonOffsetState getPolygonOffset() const;
	PolygonModeState getPolygonMode() const;

	u8 getColorMask() const;

	//! Setters
	void setRenderTarget(RenderTarget *rt);
	void setProgram(u32 programID);
	void activateUnit(u32 index);
	bool setTextureUnit(u32 index, const ITexture *texture);
	void removeTexture(ITexture *texture);

	void enableBlend(bool blend);
	void setBlendColor(const SColorf &color);
	void setBlendFunc(E_BLEND_FACTOR srcfunc, E_BLEND_FACTOR destfunc);
	void setBlendSeparateFunc(E_BLEND_FACTOR srcrgb_func, E_BLEND_FACTOR destrgb_func,
		E_BLEND_FACTOR srca_func, E_BLEND_FACTOR desta_func);
	void setBlendOp(E_BLEND_OPERATION op);

	void enableDepthTest(bool depthtest);
	void setDepthMask(bool depthmask);
	void setDepthFunc(E_COMPARISON_FUNC depthfunc);

	void enableCullFace(bool cullface);
	void setCullMode(E_CULL_MODE cullmode);

	void enableStencilTest(bool stenciltest);
	void setStencilFunc(E_COMPARISON_FUNC stencilfunc, s32 ref, u32 mask);
	void setStencilMask(u32 stencilmask);
	void setStencilOp(E_STENCIL_OP _sfail_op, E_STENCIL_OP _dpfail_op, E_STENCIL_OP _dppass_op);

	void enableScissorTest(bool scissortest);
	void setScissorBox(const core::recti &box);

	void enablePolygonOffset(bool polygonoffset);
	void setPolygonOffsetParams(f32 slope_scaled, f32 depth_bias);

	void setPolygonMode(E_CULL_MODE face, E_POLYGON_MODE mode);

	void setPointSize(f32 pointsize);
	void setLineWidth(f32 linewidth);

	void enableSampleCoverage(bool samplecoverage);

	void setViewportSize(core::recti viewportSize);

	void clearBuffers(u16 flags, video::SColor color={255, 0, 0, 0}, f32 depth=1.0f, u8 stencil=0);

	void setColorMask(u8 mask);
private:
	void initContext();
};

}
