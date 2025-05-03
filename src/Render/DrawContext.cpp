#include "DrawContext.h"

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
    ctxt->setBlendColor(img::colorf(img::PF_RGBA32F, 1.0f, 1.0f, 1.0f, 1.0f));
    ctxt->setBlendFunc(BF_SRC_COLOR, BF_DST_COLOR);
    ctxt->setBlendOp(BO_ADD);
}

static void setSubtractMode(DrawContext *ctxt)
{
    ctxt->enableBlend(true);
    ctxt->setBlendColor(img::colorf(img::PF_RGBA32F, 1.0f, 1.0f, 1.0f, 1.0f));
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

Texture *DrawContext::getActiveUnit(u32 index) const
{
	if (index >= activeUnits.size()) {
		ErrorStream << "DrawContext::getActiveUnit() indexing the active unit out of the range\n";
		return nullptr;
	}

	return activeUnits[index];
}

std::vector<Texture *> DrawContext::getActiveUnits() const
{
	return activeUnits;
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


//! Setters
void DrawContext::setFrameBuffer(FrameBuffer *fbo)
{
	if (fbo && curFBO != fbo) {
		fbo->bind();
		curFBO = fbo;
	}
}

void DrawContext::setShader(Shader *shader)
{
	if (shader && curShader != shader) {
		shader->use();
		curShader = shader;
	}
}

void DrawContext::setMesh(Mesh *mesh)
{
	if (mesh && curMesh != mesh) {
		mesh->bind();
		curMesh = mesh;
	}
}

void DrawContext::setUniformBuffer(UniformBuffer *ubo)
{
	if (ubo && curUBO != ubo) {
		ubo->bind();
		curUBO = ubo;
	}
}

void DrawContext::setActiveUnit(u32 index, Texture *texture)
{
	if (index >= activeUnits.size()) {
		ErrorStream << "DrawContext::setActiveUnit() setting the new active unit index out of the range\n";
		return;
	}

    if (texture && activeUnits[index] != texture) {
		glActiveTexture(GL_TEXTURE0 + index);
		texture->bind();
        activeUnits[index] = texture;
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
    }
}

void DrawContext::setBlendOp(BlendOp op)
{
	if (!curBlend.enabled)
	    return;
    if (curBlend.op != op) {
        glBlendEquation(toGLBlendOp[op]);

        curBlend.op = op;
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
	}
}

void DrawContext::setDepthMask(bool depthmask)
{
	if (!curDepthTest.enabled)
		return;
	if (curDepthTest.mask != depthmask) {
		glDepthMask(depthmask);
		curDepthTest.mask = depthmask;
	}
}

void DrawContext::setDepthFunc(CompareFunc depthfunc)
{
	if (!curDepthTest.enabled)
		return;
	if (curDepthTest.func != depthfunc) {
		glDepthFunc(toGLCompareFunc[depthfunc]);
		curDepthTest.func = depthfunc;
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
	}
}

void DrawContext::setCullMode(CullMode cullmode)
{
	if (!curCullFace.enabled)
		return;
	if (curCullFace.mode != cullmode) {
		glCullFace(toGLCullMode[cullmode]);
		curCullFace.mode = cullmode;
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
	}
}

void DrawContext::setStencilMask(u32 stencilmask)
{
	if (!curStencilTest.enabled)
		return;
	if (curStencilTest.mask != stencilmask) {
        glStencilMask(stencilmask);
		curStencilTest.mask = stencilmask;
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
	}
}

void DrawContext::enableScissorTest(bool scissortest)
{
	if (curScissorTest.enabled != scissortest) {
		if (scissortest)
			glEnable(GL_SCISSOR_TEST);
		else
			glDisable(GL_SCISSOR_TEST);

		curScissorTest.enabled = scissortest;
	}
}
void DrawContext::setScissorBox(const utils::recti &box)
{
	if (!curScissorTest.enabled)
		return;
	if (curScissorTest.box != box) {
        glScissor(box.ULC.X, box.ULC.Y, box.getWidth(), box.getHeight());
		curScissorTest.box = box;
	}
}

void DrawContext::setPointSize(f32 pointsize)
{
	if (pointSize != pointsize) {
		glPointSize(pointsize);
		pointSize = pointsize;
	}
}

void DrawContext::setLineWidth(f32 linewidth)
{
	if (lineWidth != linewidth) {
		glLineWidth(linewidth);
		lineWidth = linewidth;
	}
}

void DrawContext::setViewportSize(utils::recti viewportSize)
{
	if (viewport != viewportSize) {
        glViewport(viewportSize.ULC.X, viewportSize.ULC.Y, viewportSize.getWidth(), viewportSize.getHeight());
		viewport = viewportSize;
	}
}

void DrawContext::initContext(utils::recti viewportSize)
{
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);

	glPixelStorei(GL_PACK_ALIGNMENT, 1);

    setBlendMode(GLBlendMode::ALPHA);

	enableCullFace(curCullFace.enabled);
	setCullMode(curCullFace.mode);

	enableDepthTest(curDepthTest.enabled);
	setDepthFunc(curDepthTest.func);
	setDepthMask(curDepthTest.mask);

	setViewportSize(viewportSize);
}

}
