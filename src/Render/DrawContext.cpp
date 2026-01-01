#include "DrawContext.h"
#include "Common.h"
#include "OpenGLIncludes.h"

namespace render
{

// Standard blend modes setup functions
static void setNormalMode(DrawContext *ctxt)
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
}


//! Getters
FrameBuffer *DrawContext::getFrameBuffer() const
{
	return curFBO;
}

Shader *DrawContext::getShader() const
{
	return curShader;
}

Mesh *DrawContext::getMesh() const
{
	return curMesh;
}

UniformBuffer *DrawContext::getUniformBuffer() const
{
	return curUBO;
}

Texture *DrawContext::getTextureUnit(u32 index) const
{
    if (index >= textureUnits.size()) {
		ErrorStream << "DrawContext::getActiveUnit() indexing the active unit out of the range\n";
		return nullptr;
	}

    return textureUnits[index];
}

std::vector<Texture *> DrawContext::getTextureUnits() const
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
void DrawContext::setFrameBuffer(FrameBuffer *fbo)
{
	if (fbo && curFBO != fbo) {
		fbo->bind();
		curFBO = fbo;

        TEST_GL_ERROR();
	}
}

void DrawContext::setShader(Shader *shader)
{
	if (shader && curShader != shader) {
		shader->use();
		curShader = shader;

        TEST_GL_ERROR();
	}
}

void DrawContext::setMesh(Mesh *mesh)
{
	if (mesh && curMesh != mesh) {
		mesh->bind();
		curMesh = mesh;

        TEST_GL_ERROR();
	}
}

void DrawContext::setUniformBuffer(UniformBuffer *ubo)
{
	if (ubo && curUBO != ubo) {
		ubo->bind();
		curUBO = ubo;

        TEST_GL_ERROR();
	}
}

void DrawContext::setActiveUnit(u32 index, Texture *texture)
{
    if (index >= textureUnits.size()) {
		ErrorStream << "DrawContext::setActiveUnit() setting the new active unit index out of the range\n";
		return;
	}

    if (!curShader) {
        ErrorStream << "DrawContext::setActiveUnit() shader must be bound\n";
        return;
    }

    if (textureUnits[index] != texture) {
        if (activeUnit != index) {
            glActiveTexture(GL_TEXTURE0 + index);
            activeUnit = index;
            TEST_GL_ERROR();
        }

        if (textureUnits[index]) {
            textureUnits[index]->unbind();
            textureUnits[index] = nullptr;
        }
        if (texture) {
            texture->bind();
            textureUnits[index] = texture;
        }

        curShader->setSampler(index);
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

        TEST_GL_ERROR();
    }
}

void DrawContext::setBlendMode(GLBlendMode mode)
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
}

void DrawContext::setBlendColor(const img::colorf &color)
{
	if (!curBlend.enabled)
	    return;
    if (curBlend.color != color) {
        glBlendColor(color.R(), color.G(), color.B(), color.A());

        curBlend.color = color;

        TEST_GL_ERROR();
    }
}

void DrawContext::setBlendFunc(BlendFunc srcfunc, BlendFunc destfunc)
{
	if (!curBlend.enabled)
	    return;
    if (curBlend.func_srcrgb != srcfunc || curBlend.func_destrgb != destfunc) {
        glBlendFunc(toGLBlendFunc[srcfunc], toGLBlendFunc[destfunc]);

        curBlend.func_srcrgb = srcfunc;
        curBlend.func_destrgb = destfunc;

        TEST_GL_ERROR();
    }
}

void DrawContext::setBlendSeparateFunc(BlendFunc srcrgb_func, BlendFunc destrgb_func,
    BlendFunc srca_func, BlendFunc desta_func)
{
	if (!curBlend.enabled)
	    return;
    if (curBlend.func_srcrgb != srcrgb_func || curBlend.func_destrgb != destrgb_func ||
            curBlend.func_srca != srca_func || curBlend.func_desta != desta_func) {
        glBlendFuncSeparate(toGLBlendFunc[srcrgb_func], toGLBlendFunc[destrgb_func],
            toGLBlendFunc[srca_func], toGLBlendFunc[desta_func]);

        curBlend.func_srcrgb = srcrgb_func;
        curBlend.func_destrgb = destrgb_func;
        curBlend.func_srca = srca_func;
        curBlend.func_desta = desta_func;

        TEST_GL_ERROR();
    }
}

void DrawContext::setBlendOp(BlendOp op)
{
	if (!curBlend.enabled)
	    return;
    if (curBlend.op != op) {
        glBlendEquation(toGLBlendOp[op]);

        curBlend.op = op;

        TEST_GL_ERROR();
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

        TEST_GL_ERROR();
	}
}

void DrawContext::setDepthMask(bool depthmask)
{
	if (!curDepthTest.enabled)
		return;
	if (curDepthTest.mask != depthmask) {
		glDepthMask(depthmask);
		curDepthTest.mask = depthmask;

        TEST_GL_ERROR();
	}
}

void DrawContext::setDepthFunc(CompareFunc depthfunc)
{
	if (!curDepthTest.enabled)
		return;
	if (curDepthTest.func != depthfunc) {
		glDepthFunc(toGLCompareFunc[depthfunc]);
		curDepthTest.func = depthfunc;

        TEST_GL_ERROR();
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

        TEST_GL_ERROR();
	}
}

void DrawContext::setCullMode(CullMode cullmode)
{
	if (!curCullFace.enabled)
		return;
	if (curCullFace.mode != cullmode) {
		glCullFace(toGLCullMode[cullmode]);
		curCullFace.mode = cullmode;

        TEST_GL_ERROR();
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

        TEST_GL_ERROR();
	}
}

void DrawContext::setStencilFunc(CompareFunc stencilfunc, s32 ref, u32 mask)
{
	if (!curStencilTest.enabled)
		return;
	if (curStencilTest.func != stencilfunc) {
		glStencilFunc(toGLCompareFunc[stencilfunc], ref, mask);
		curStencilTest.func = stencilfunc;
		curStencilTest.ref = ref;
		curStencilTest.mask = mask;

        TEST_GL_ERROR();
	}
}

void DrawContext::setStencilMask(u32 stencilmask)
{
	if (!curStencilTest.enabled)
		return;
	if (curStencilTest.mask != stencilmask) {
        glStencilMask(stencilmask);
		curStencilTest.mask = stencilmask;

        TEST_GL_ERROR();
	}
}

void DrawContext::setStencilOp(StencilOp _sfail_op, StencilOp _dpfail_op, StencilOp _dppass_op)
{
	if (!curStencilTest.enabled)
		return;
	if (curStencilTest.sfail_op != _sfail_op || curStencilTest.dpfail_op != _dpfail_op ||
		curStencilTest.dppass_op != _dppass_op) {
		glStencilOp(toGLStencilOp[_sfail_op], toGLStencilOp[_dpfail_op], toGLStencilOp[_dppass_op]);

		curStencilTest.sfail_op = _sfail_op;
		curStencilTest.dpfail_op = _dpfail_op;
		curStencilTest.dppass_op = _dppass_op;

        TEST_GL_ERROR();
	}
}

void DrawContext::enableScissorTest(bool scissortest)
{
    if (scissortest)
        glEnable(GL_SCISSOR_TEST);
    else
        glDisable(GL_SCISSOR_TEST);

    curScissorTest.enabled = scissortest;

    TEST_GL_ERROR();
}

void DrawContext::setScissorBox(const recti &box)
{
	if (!curScissorTest.enabled)
		return;

    glScissor(box.ULC.X, viewport.getHeight()-box.LRC.Y, abs(box.getWidth()), abs(box.getHeight()));

    curScissorTest.box = box;

    TEST_GL_ERROR();
}

void DrawContext::enablePolygonOffset(bool polygonoffset)
{
    if (curPolygonOffset.enabled != polygonoffset) {
        if (polygonoffset)
            glEnable(GL_POLYGON_OFFSET_FILL);
        else
            glDisable(GL_POLYGON_OFFSET_FILL);

        curPolygonOffset.enabled = polygonoffset;

        TEST_GL_ERROR();
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

        TEST_GL_ERROR();
    }
}

void DrawContext::setPolygonMode(CullMode face, PolygonMode mode)
{
    if (curPolygonMode.face != face || curPolygonMode.mode != mode) {
        glPolygonMode(toGLCullMode[face], toGLPolygonMode[mode]);

        curPolygonMode.face = face;
        curPolygonMode.mode = mode;

        TEST_GL_ERROR();
    }
}

void DrawContext::setPointSize(f32 pointsize)
{
	if (pointSize != pointsize) {
		glPointSize(pointsize);
		pointSize = pointsize;

        TEST_GL_ERROR();
	}
}

void DrawContext::setLineWidth(f32 linewidth)
{
	if (lineWidth != linewidth) {
		glLineWidth(linewidth);
		lineWidth = linewidth;

        TEST_GL_ERROR();
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

        TEST_GL_ERROR();
    }
}

void DrawContext::setViewportSize(recti viewportSize)
{
	if (viewport != viewportSize) {
        glViewport(viewportSize.ULC.X, viewportSize.ULC.Y, viewportSize.getWidth(), viewportSize.getHeight());
		viewport = viewportSize;

        TEST_GL_ERROR();
	}
}

void DrawContext::clearBuffers(u16 flags, img::color8 color, f32 depth, u8 stencil)
{
    GLbitfield mask = 0;

    //u8 prevColorMask = colorMask;
    //bool prevDepthMask = curDepthTest.mask;

    if (flags & CBF_COLOR) {
        //setColorMask(CP_ALL);
        //TEST_GL_ERROR();

        f32 inv = 1.0f / 255.0f;

        glClearColor(color.R() * inv, color.G() * inv, color.B() * inv, color.A() * inv);
        TEST_GL_ERROR();
        mask |= GL_COLOR_BUFFER_BIT;
    }

    if (flags & CBF_DEPTH) {
        //setDepthMask(true);
        //TEST_GL_ERROR();

        glClearDepth(depth);
        TEST_GL_ERROR();
        mask |= GL_DEPTH_BUFFER_BIT;
    }

    if (flags & CBF_STENCIL) {
        glClearStencil(stencil);
        TEST_GL_ERROR();
        mask |= GL_STENCIL_BUFFER_BIT;
    }

    if (mask) {
        glClear(mask);
        TEST_GL_ERROR();
    }

    //setColorMask(prevColorMask);
    //TEST_GL_ERROR();
    //setDepthMask(prevDepthMask);
    //TEST_GL_ERROR();
}

void DrawContext::setColorMask(u8 mask)
{
    if (mask != colorMask) {
        glColorMask(
            (mask & CP_RED) ? GL_TRUE : GL_FALSE,
            (mask & CP_GREEN) ? GL_TRUE : GL_FALSE,
            (mask & CP_BLUE) ? GL_TRUE : GL_FALSE,
            (mask & CP_ALPHA) ? GL_TRUE : GL_FALSE);

        colorMask = mask;

        TEST_GL_ERROR();
    }
}

void DrawContext::initContext(recti viewportSize)
{
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

    setBlendMode(GLBlendMode::ALPHA);

    setPointSize(1.0f);
    setLineWidth(1.0f);

	enableCullFace(curCullFace.enabled);
	setCullMode(curCullFace.mode);

	enableDepthTest(curDepthTest.enabled);
	setDepthFunc(curDepthTest.func);
	setDepthMask(curDepthTest.mask);

	setViewportSize(viewportSize);
}

}
