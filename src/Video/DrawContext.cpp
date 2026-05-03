#include "Video/DrawContext.h"
#include "Video/RenderTarget.h"
#include "Video/VideoDriver.h"
#include "Video/Texture.h"
#include "Common.h"
#include "Video/OpenGLFeatures.h"
#include "Device/Logger.h"

namespace video
{

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

DrawContext::DrawContext(VideoDriver *_driver)
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

const GLTexture *DrawContext::getTextureUnit(u32 index) const
{
    if (index >= textureUnits.size()) {
		g_irrlogger->log("DrawContext::getActiveUnit() indexing the active unit out of the range", ELL_ERROR);
		return nullptr;
	}

    return textureUnits[index];
}

std::vector<const GLTexture *> DrawContext::getTextureUnits() const
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

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::activateUnit(u32 index)
{
	if (activeUnit != index) {
        glActiveTexture(GL_TEXTURE0 + index);
		activeUnit = index;
        TEST_GL_ERROR(driver);
	}
}

bool DrawContext::setTextureUnit(u32 index, const GLTexture *texture)
{
    if (index >= textureUnits.size()) {
		g_irrlogger->log("DrawContext::setActiveUnit() setting the new active unit index out of the range", ELL_ERROR);
		return false;
	}

	activateUnit(index);

    if (textureUnits[index] != texture) {
        if (textureUnits[index]) {
            textureUnits[index]->unbind();

			textureUnits[index]->drop();
            textureUnits[index] = nullptr;
        }
        if (texture) {
            texture->bind();

			texture->grab();
            textureUnits[index] = texture;
        }
	}

	return true;
}

void DrawContext::removeTexture(GLTexture *texture)
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

        TEST_GL_ERROR(driver);
    }
}

void DrawContext::setBlendColor(const SColorf &color)
{
	if (!curBlend.enabled)
	    return;
    if (curBlend.color != color) {
		glBlendColor(color.r, color.g, color.b, color.a);

        curBlend.color = color;

        TEST_GL_ERROR(driver);
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

        TEST_GL_ERROR(driver);
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

            TEST_GL_ERROR(driver);
		}
	}
	else {
		setBlendFunc(srcrgb, destrgb);

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::setBlendOp(E_BLEND_OPERATION op)
{
	if (!curBlend.enabled)
	    return;
	if (curBlend.mode != op) {
		glBlendEquation(toGLBlendOp[op]);

		curBlend.mode = op;

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::setBlendMode(E_BLEND_MODE mode)
{
	enableBlend(mode != EBM_NONE);

	switch(mode) {
	case EBM_ALPHA:
		setBlendFunc(EBF_SRC_ALPHA, EBF_ONE_MINUS_SRC_ALPHA);
		setBlendOp(EBO_ADD);
		break;
	case EBM_ADD:
		setBlendFunc(EBF_ONE, EBF_ONE);
		setBlendOp(EBO_ADD);
		break;
	case EBM_SUBTRACT:
		setBlendFunc(EBF_ONE, EBF_ONE);
		setBlendOp(EBO_SUBTRACT);
		break;
	case EBM_REVSUBTRACT:
		setBlendFunc(EBF_ONE, EBF_ONE);
		setBlendOp(EBO_REVSUBTRACT);
		break;
	case EBM_MULTIPLY:
		setBlendSeparateFunc(EBF_DST_COLOR, EBF_ZERO, EBF_DST_ALPHA, EBF_ZERO);
		setBlendOp(EBO_ADD);
		break;
	case EBM_SCREEN:
		setBlendSeparateFunc(EBF_ONE, EBF_ONE_MINUS_SRC_COLOR, EBF_ONE, EBF_ONE_MINUS_SRC_ALPHA);
		setBlendOp(EBO_ADD);
		break;
	case EBM_MIN:
		setBlendOp(EBO_MIN);
		break;
	case EBM_MAX:
		setBlendOp(EBO_MAX);
		break;
	default:
		break;
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

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::setDepthMask(bool depthmask)
{
	if (!curDepthTest.enabled)
		return;
	if (curDepthTest.mask != depthmask) {
		glDepthMask(depthmask);
		curDepthTest.mask = depthmask;

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::setDepthFunc(E_COMPARISON_FUNC depthfunc)
{
	if (!curDepthTest.enabled)
		return;
	if (curDepthTest.func != depthfunc) {
		glDepthFunc(toGLCompareFunc[depthfunc]);
		curDepthTest.func = depthfunc;

        TEST_GL_ERROR(driver);
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

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::setCullMode(E_CULL_MODE cullmode)
{
	if (!curCullFace.enabled)
		return;
	if (curCullFace.mode != cullmode) {
		glCullFace(toGLCullMode[cullmode]);
		curCullFace.mode = cullmode;

        TEST_GL_ERROR(driver);
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

        TEST_GL_ERROR(driver);
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

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::setStencilMask(u32 stencilmask)
{
	if (!curStencilTest.enabled)
		return;
	if (curStencilTest.mask != stencilmask) {
        glStencilMask(stencilmask);
		curStencilTest.mask = stencilmask;

        TEST_GL_ERROR(driver);
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

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::enableScissorTest(bool scissortest)
{
	if (scissortest)
		glEnable(GL_SCISSOR_TEST);
	else
		glDisable(GL_SCISSOR_TEST);

	curScissorTest.enabled = scissortest;

    TEST_GL_ERROR(driver);
}

void DrawContext::setScissorBox(s32 x, s32 y, s32 w, s32 h)
{
	if (!curScissorTest.enabled)
		return;

	glScissor(x, y, w, h);

	curScissorTest.box = {x, y, x+w, y+h};

    TEST_GL_ERROR(driver);
}

void DrawContext::enablePolygonOffset(bool polygonoffset)
{
	if (curPolygonOffset.enabled != polygonoffset) {
		if (polygonoffset)
			glEnable(GL_POLYGON_OFFSET_FILL);
		else
			glDisable(GL_POLYGON_OFFSET_FILL);

		curPolygonOffset.enabled = polygonoffset;

        TEST_GL_ERROR(driver);
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

        TEST_GL_ERROR(driver);
    }
}

void DrawContext::setPolygonMode(E_CULL_MODE face, E_POLYGON_MODE mode)
{
#ifdef _IRR_COMPILE_WITH_OPENGL3_
	if (curPolygonMode.face != face || curPolygonMode.mode != mode) {
		glPolygonMode(toGLCullMode[face], toGLPolygonMode[mode]);

		curPolygonMode.face = face;
		curPolygonMode.mode = mode;

        TEST_GL_ERROR(driver);
	}
#endif
}

void DrawContext::setPointSize(f32 pointsize)
{
	if (pointSize != pointsize) {
		glPointSize(pointsize);
		pointSize = pointsize;

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::setLineWidth(f32 linewidth)
{
	if (lineWidth != linewidth) {
		glLineWidth(linewidth);
		lineWidth = linewidth;

        TEST_GL_ERROR(driver);
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

        TEST_GL_ERROR(driver);
	}
}

void DrawContext::setViewport(s32 x, s32 y, s32 w, s32 h)
{
	core::recti targetRect(x, y, x+w, y+h);
	if (viewport != targetRect) {
		glViewport(x, y, w, h);
		viewport = targetRect;

        TEST_GL_ERROR(driver);
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

        TEST_GL_ERROR(driver);
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
	setViewport(0, 0, ScreenSize.Width, ScreenSize.Height);
}

}
