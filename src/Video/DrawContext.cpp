#include "DrawContext.h"
#include "RenderTarget.h"
#include "IVideoDriver.h"
#include "Common.h"
#include "COpenGLCoreTexture.h"
#include "Logger.h"

namespace video
{

// Standard blend modes setup functions
/*static void setNormalMode(DrawContext *ctxt)
{
    ctxt->setBlendFunc(BF_ONE, BF_ZERO);
    ctxt->setBlendOp(BO_ADD);
}

static void setAlphaMode(DrawContext *ctxt)
{
    ctxt->enableBlend(true);
    ctxt->setBlendFunc(BF_SRC_ALPHA, BF_ONE_MINUS_SRC_ALPHA);
    ctxt->setBlendOp(BO_ADD);
}

static void setAddMode(DrawContext *ctxt)
{
    ctxt->enableBlend(true);
    ctxt->setBlendColor(img::colorf(1.0f, 1.0f, 1.0f, 1.0f));
    ctxt->setBlendFunc(BF_SRC_COLOR, BF_DST_COLOR);
    ctxt->setBlendOp(BO_ADD);
}

static void setSubtractMode(DrawContext *ctxt)
{
    ctxt->enableBlend(true);
    ctxt->setBlendColor(img::colorf(1.0f, 1.0f, 1.0f, 1.0f));
    ctxt->setBlendFunc(BF_SRC_COLOR, BF_DST_COLOR);
    ctxt->setBlendOp(BO_SUBTRACT);
}*/

std::array<GLenum, EBO_COUNT> toGLBlendOp = {
	0,
	GL_FUNC_ADD,
	GL_FUNC_SUBTRACT,
	GL_FUNC_REVERSE_SUBTRACT,
	GL_MIN,
	GL_MAX
};

std::array<GLenum, EBF_COUNT> toGLBlendFunc = {
	GL_ZERO,
	GL_ONE,
	GL_DST_COLOR,
	GL_ONE_MINUS_DST_COLOR,
	GL_SRC_COLOR,
	GL_ONE_MINUS_SRC_COLOR,
	GL_SRC_ALPHA,
	GL_ONE_MINUS_SRC_ALPHA,
	GL_DST_ALPHA,
	GL_ONE_MINUS_DST_ALPHA,
	GL_SRC_ALPHA_SATURATE
};

std::array<GLenum, ECFN_COUNT> toGLCompareFunc = {
	0,
	GL_LEQUAL,
	GL_EQUAL,
	GL_LESS,
	GL_NOTEQUAL,
	GL_GEQUAL,
	GL_GREATER,
	GL_ALWAYS,
	GL_NEVER
};

std::array<GLenum, ECM_COUNT> toGLCullMode = {
	GL_BACK,
	GL_FRONT,
	GL_FRONT_AND_BACK
};

std::array<GLenum, ESO_COUNT> toGLStencilOp = {
	GL_KEEP,
	GL_ZERO,
	GL_REPLACE,
	GL_INVERT
};

std::array<GLenum, EPM_COUNT> toGLPolygonMode
{
	GL_FILL,
	GL_POINT,
	GL_LINE
};

DrawContext::DrawContext(IVideoDriver *_driver)
	: driver(_driver), maxTextureUnits(driver->getFeatures().MaxTextureUnits)
{
	textureUnits.resize(maxTextureUnits, nullptr);

	initContext();
}

DrawContext::~DrawContext()
{
	for (u8 i = 0; i < textureUnits.size(); i++) {
		if (textureUnits[i])
			textureUnits[i]->drop();
	}
}

//! Getters
RenderTarget *DrawContext::getRenderTarget() const
{
	return curRT;
}

u32 DrawContext::getProgram() const
{
	return curProgramID;
}

const ITexture *DrawContext::getTextureUnit(u32 index) const
{
    if (index >= textureUnits.size()) {
		g_irrlogger->log("DrawContext::getActiveUnit() indexing the active unit out of the range", ELL_ERROR);
		return nullptr;
	}

    return textureUnits[index];
}

std::vector<const ITexture *> DrawContext::getTextureUnits() const
{
    return textureUnits;
}

BlendState DrawContext::getBlendState() const
{
    return curBlend;
}

DepthTestState DrawContext::getDepthTest() const
{
	return curDepthTest;
}

CullFaceState DrawContext::getCullFace() const
{
	return curCullFace;
}

StencilTestState DrawContext::getStencilTest() const
{
	return curStencilTest;
}

ScissorTestState DrawContext::getScissorTest() const
{
	return curScissorTest;
}

PolygonOffsetState DrawContext::getPolygonOffset() const
{
    return curPolygonOffset;
}

PolygonModeState DrawContext::getPolygonMode() const
{
    return curPolygonMode;
}

u8 DrawContext::getColorMask() const
{
    return colorMask;
}

//! Setters
void DrawContext::setRenderTarget(RenderTarget *rt)
{
	if (curRT == rt)
		return;

	if (rt) {
		rt->bind();
		curRT = rt;
	}
	else if (curRT) {
		curRT->unbind();
		curRT = nullptr;
	}
}

void DrawContext::setProgram(u32 programID)
{
	if (programID && curProgramID != programID) {
		glUseProgram(programID);
		curProgramID = programID;

		driver->testGLError();
	}
}

void DrawContext::activateUnit(u32 index)
{
	if (activeUnit != index) {
		glActiveTexture(GL_TEXTURE0 + index);
		activeUnit = index;
		driver->testGLError();
	}
}

bool DrawContext::setTextureUnit(u32 index, const ITexture *texture)
{
    if (index >= textureUnits.size()) {
		g_irrlogger->log("DrawContext::setActiveUnit() setting the new active unit index out of the range", ELL_ERROR);
		return false;
	}

	activateUnit(index);

    if (textureUnits[index] != texture) {
        if (textureUnits[index]) {
			auto texImpl = static_cast<const COpenGL3Texture *>(textureUnits[index]);
			glBindTexture(texImpl->getOpenGLTextureType(), 0);
			driver->testGLError();

			textureUnits[index]->drop();
            textureUnits[index] = nullptr;
        }
        if (texture) {
			auto texImpl = static_cast<const COpenGL3Texture *>(texture);
			glBindTexture(texImpl->getOpenGLTextureType(), texImpl->getID());
			driver->testGLError();

			texture->grab();
            textureUnits[index] = texture;
        }
	}

	return true;
}

void DrawContext::removeTexture(ITexture *texture)
{
	if (!texture)
		return;

	for (u8 i = 0; i < textureUnits.size(); i++) {
		if (textureUnits[i] == texture) {
			textureUnits[i]->drop();
			textureUnits[i] = nullptr;
		}
	}
}

void DrawContext::enableBlend(bool blend)
{
    if (curBlend.enabled != blend) {
        if (blend)
            glEnable(GL_BLEND);
        else
            glDisable(GL_BLEND);

        curBlend.enabled = blend;

		driver->testGLError();
    }
}

/*void DrawContext::setBlendMode(GLBlendMode mode)
{
    switch (mode) {
    case GLBlendMode::NORMAL:
        setNormalMode(this);
        break;
    case GLBlendMode::ALPHA:
        setAlphaMode(this);
        break;
    case GLBlendMode::ADD:
        setAddMode(this);
        break;
    case GLBlendMode::SUBTRACT:
        setSubtractMode(this);
        break;
    default:
        break;
    }

    curBlend.mode = mode;
}*/

void DrawContext::setBlendColor(const SColorf &color)
{
	if (!curBlend.enabled)
	    return;
    if (curBlend.color != color) {
		glBlendColor(color.r, color.g, color.b, color.a);

        curBlend.color = color;

		driver->testGLError();
    }
}

void DrawContext::setBlendFunc(E_BLEND_FACTOR src, E_BLEND_FACTOR dest)
{
	if (!curBlend.enabled)
	    return;
	if (curBlend.func_srcrgb != src || curBlend.func_destrgb != dest ||
			curBlend.func_srca != src || curBlend.func_destrgb != dest) {
		glBlendFunc(toGLBlendFunc[src], toGLBlendFunc[dest]);

		curBlend.func_srcrgb = src;
		curBlend.func_destrgb = dest;
		curBlend.func_srca = src;
		curBlend.func_desta = dest;

		driver->testGLError();
    }
}

void DrawContext::setBlendSeparateFunc(E_BLEND_FACTOR srcrgb, E_BLEND_FACTOR destrgb,
	E_BLEND_FACTOR srca, E_BLEND_FACTOR desta)
{
	if (!curBlend.enabled)
	    return;
	if (srcrgb != srca || destrgb != desta) {
		if (curBlend.func_srcrgb != srcrgb || curBlend.func_destrgb != destrgb ||
				curBlend.func_srca != srca || curBlend.func_desta != desta) {
			glBlendFuncSeparate(toGLBlendFunc[srcrgb], toGLBlendFunc[destrgb],
				toGLBlendFunc[srca], toGLBlendFunc[desta]);

			curBlend.func_srcrgb = srcrgb;
			curBlend.func_destrgb = destrgb;
			curBlend.func_srca = srca;
			curBlend.func_desta = desta;

			driver->testGLError();
		}
	}
	else {
		setBlendFunc(srcrgb, destrgb);

		driver->testGLError();
	}
}

void DrawContext::setBlendOp(E_BLEND_OPERATION op)
{
	if (!curBlend.enabled)
	    return;
	if (curBlend.mode != op) {
        glBlendEquation(toGLBlendOp[op]);

		curBlend.mode = op;

		driver->testGLError();
    }
}

void DrawContext::enableDepthTest(bool depthtest)
{
    if (curDepthTest.enabled != depthtest) {
        if (depthtest)
			glEnable(GL_DEPTH_TEST);
		else
			glDisable(GL_DEPTH_TEST);

		curDepthTest.enabled = depthtest;

		driver->testGLError();
	}
}

void DrawContext::setDepthMask(bool depthmask)
{
	if (!curDepthTest.enabled)
		return;
	if (curDepthTest.mask != depthmask) {
		glDepthMask(depthmask);
		curDepthTest.mask = depthmask;

		driver->testGLError();
	}
}

void DrawContext::setDepthFunc(E_COMPARISON_FUNC depthfunc)
{
	if (!curDepthTest.enabled)
		return;
	if (curDepthTest.func != depthfunc) {
		glDepthFunc(toGLCompareFunc[depthfunc]);
		curDepthTest.func = depthfunc;

		driver->testGLError();
	}
}

void DrawContext::enableCullFace(bool cullface)
{
	if (curCullFace.enabled != cullface) {
		if (cullface)
			glEnable(GL_CULL_FACE);
		else
			glDisable(GL_CULL_FACE);

		curCullFace.enabled = cullface;

		driver->testGLError();
	}
}

void DrawContext::setCullMode(E_CULL_MODE cullmode)
{
	if (!curCullFace.enabled)
		return;
	if (curCullFace.mode != cullmode) {
		glCullFace(toGLCullMode[cullmode]);
		curCullFace.mode = cullmode;

		driver->testGLError();
	}
}

void DrawContext::enableStencilTest(bool stenciltest)
{
	if (curStencilTest.enabled != stenciltest) {
		if (stenciltest)
			glEnable(GL_STENCIL_TEST);
		else
			glDisable(GL_STENCIL_TEST);

		curStencilTest.enabled = stenciltest;

		driver->testGLError();
	}
}

void DrawContext::setStencilFunc(E_COMPARISON_FUNC stencilfunc, s32 ref, u32 mask)
{
	if (!curStencilTest.enabled)
		return;
	if (curStencilTest.func != stencilfunc) {
		glStencilFunc(toGLCompareFunc[stencilfunc], ref, mask);
		curStencilTest.func = stencilfunc;
		curStencilTest.ref = ref;
		curStencilTest.mask = mask;

		driver->testGLError();
	}
}

void DrawContext::setStencilMask(u32 stencilmask)
{
	if (!curStencilTest.enabled)
		return;
	if (curStencilTest.mask != stencilmask) {
        glStencilMask(stencilmask);
		curStencilTest.mask = stencilmask;

		driver->testGLError();
	}
}

void DrawContext::setStencilOp(E_STENCIL_OP _sfail_op, E_STENCIL_OP _dpfail_op, E_STENCIL_OP _dppass_op)
{
	if (!curStencilTest.enabled)
		return;
	if (curStencilTest.sfail_op != _sfail_op || curStencilTest.dpfail_op != _dpfail_op ||
		curStencilTest.dppass_op != _dppass_op) {
		glStencilOp(toGLStencilOp[_sfail_op], toGLStencilOp[_dpfail_op], toGLStencilOp[_dppass_op]);

		curStencilTest.sfail_op = _sfail_op;
		curStencilTest.dpfail_op = _dpfail_op;
		curStencilTest.dppass_op = _dppass_op;

		driver->testGLError();
	}
}

void DrawContext::enableScissorTest(bool scissortest)
{
    if (scissortest)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);

    curScissorTest.enabled = scissortest;

	driver->testGLError();
}

void DrawContext::setScissorBox(const core::recti &box)
{
	if (!curScissorTest.enabled)
		return;

	glScissor(
		box.UpperLeftCorner.X, viewport.getHeight()-box.LowerRightCorner.Y,
		abs(box.getWidth()), abs(box.getHeight()));

    curScissorTest.box = box;

	driver->testGLError();
}

void DrawContext::enablePolygonOffset(bool polygonoffset)
{
    if (curPolygonOffset.enabled != polygonoffset) {
        if (polygonoffset)
            glEnable(GL_POLYGON_OFFSET_FILL);
        else
            glDisable(GL_POLYGON_OFFSET_FILL);

        curPolygonOffset.enabled = polygonoffset;

		driver->testGLError();
    }
}

void DrawContext::setPolygonOffsetParams(f32 slope_scaled, f32 depth_bias)
{
    if (!curPolygonOffset.enabled)
        return;
    if (curPolygonOffset.slope_scale != slope_scaled || curPolygonOffset.depth_bias != depth_bias) {
        glPolygonOffset(slope_scaled, depth_bias);
        curPolygonOffset.slope_scale = slope_scaled;
        curPolygonOffset.depth_bias = depth_bias;

		driver->testGLError();
    }
}

void DrawContext::setPolygonMode(E_CULL_MODE face, E_POLYGON_MODE mode)
{
    if (curPolygonMode.face != face || curPolygonMode.mode != mode) {
        glPolygonMode(toGLCullMode[face], toGLPolygonMode[mode]);

        curPolygonMode.face = face;
        curPolygonMode.mode = mode;

		driver->testGLError();
    }
}

void DrawContext::setPointSize(f32 pointsize)
{
	if (pointSize != pointsize) {
		glPointSize(pointsize);
		pointSize = pointsize;

		driver->testGLError();
	}
}

void DrawContext::setLineWidth(f32 linewidth)
{
	if (lineWidth != linewidth) {
		glLineWidth(linewidth);
		lineWidth = linewidth;

		driver->testGLError();
	}
}

void DrawContext::enableSampleCoverage(bool samplecoverage)
{
    if (sampleCoverage != samplecoverage) {
        if (samplecoverage)
            glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
        else
            glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

        sampleCoverage = samplecoverage;

		driver->testGLError();
    }
}

void DrawContext::setViewportSize(core::recti viewportSize)
{
	if (viewport != viewportSize) {
		glViewport(
			viewportSize.UpperLeftCorner.X, viewportSize.UpperLeftCorner.Y,
			viewportSize.getWidth(), viewportSize.getHeight());
		viewport = viewportSize;

	   driver->testGLError();
	}
}

void DrawContext::clearBuffers(u16 flags, video::SColor color, f32 depth, u8 stencil)
{
	GLbitfield mask = 0;
	u8 colorMask = getColorMask();
	bool depthMask = getDepthTest().mask;

	if (flags & ECBF_COLOR) {
		setColorMask(ECP_ALL);

		const f32 inv = 1.0f / 255.0f;
		glClearColor(color.getRed() * inv, color.getGreen() * inv,
				color.getBlue() * inv, color.getAlpha() * inv);

		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (flags & ECBF_DEPTH) {
		setDepthMask(true);
		glClearDepthf(depth);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (flags & ECBF_STENCIL) {
		setStencilMask(stencil);
		mask |= GL_STENCIL_BUFFER_BIT;
	}

	if (mask)
		glClear(mask);

	setColorMask(colorMask);
	setDepthMask(depthMask);
}

void DrawContext::setColorMask(u8 mask)
{
    if (mask != colorMask) {
        glColorMask(
			(mask & ECP_RED) ? GL_TRUE : GL_FALSE,
			(mask & ECP_GREEN) ? GL_TRUE : GL_FALSE,
			(mask & ECP_BLUE) ? GL_TRUE : GL_FALSE,
			(mask & ECP_ALPHA) ? GL_TRUE : GL_FALSE);

        colorMask = mask;

		driver->testGLError();
    }
}

void DrawContext::initContext()
{
	if (driver->getFeatures().BlendOperation)
		glBlendEquation(GL_FUNC_ADD);

	glBlendFunc(GL_ONE, GL_ZERO);
	glDisable(GL_BLEND);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

	glCullFace(toGLCullMode[curCullFace.mode]);
	glDisable(GL_CULL_FACE);

	glDepthFunc(toGLCompareFunc[curDepthTest.func]);
	glDepthMask(GL_TRUE);
	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);

	auto ScreenSize = driver->getScreenSize();
	setViewportSize(core::recti(0, 0, ScreenSize.Width, ScreenSize.Height));
}

}
