#include "DrawContext.h"
#include <map>

namespace render
{

// The standard glBlend* functions limit the blending opportunities
static const std::vector<img::BlendMode> supportedBlendModes = {
	img::BM_NORMAL,
	img::BM_ALPHA,
	img::BM_ADD,
    img::BM_SUBTRACTION
};

// Various blend modes setup functions
static void setNormalMode()
{
	glBlendFunc(toGLBlendFunc[BF_ONE], toGLBlendFunc[BF_ZERO]);
	glBlendEquation(toGLBlendOp[BO_ADD]);
}

static void setAlphaMode()
{
	glBlendFunc(toGLBlendFunc[BF_SRC_ALPHA], toGLBlendFunc[BF_ONE_MINUS_SRC_ALPHA]);
	glBlendEquation(toGLBlendOp[BO_ADD]);
}

static void setAddMode()
{
	glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(toGLBlendFunc[BF_SRC_COLOR], toGLBlendFunc[BF_DST_COLOR]);
	glBlendEquation(toGLBlendOp[BO_ADD]);
}

static void setSubtractMode()
{
	glBlendColor(1.0f, 1.0f, 1.0f, 1.0f);
	glBlendFunc(toGLBlendFunc[BF_SRC_COLOR], toGLBlendFunc[BF_DST_COLOR]);
	glBlendEquation(toGLBlendOp[BO_SUBTRACT]);
}

// Mapping the blend mode to the corresponding setup function
static const std::map<img::BlendMode, std::function<void()>> setupBlendFunctions = {
	{img::BM_NORMAL, &setNormalMode},
	{img::BM_ALPHA, &setAlphaMode},
	{img::BM_ADD, &setAddMode},
    {img::BM_SUBTRACTION, &setSubtractMode}
};


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
		SDL_LogError(LC_VIDEO, "DrawContext::getActiveUnit() indexing the active unit out of the range");
		return nullptr;
	}

	return activeUnits[index];
}

std::vector<Texture *> DrawContext::getActiveUnits() const
{
	return activeUnits;
}

img::BlendMode DrawContext::getBlendMode() const
{
	return curMode;
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
		SDL_LogError(LC_VIDEO, "DrawContext::setActiveUnit() setting the new active unit index out of the range");
		return;
	}

    if (texture && activeUnits[index] != texture) {
		glActiveTexture(GL_TEXTURE0 + index);
		texture->bind();
        activeUnits[index] = texture;
	}
}

void DrawContext::setBlendMode(img::BlendMode blendmode)
{
	auto supported_mode = std::find(supportedBlendModes.begin(), supportedBlendModes.end(), blendmode);

	if (supported_mode == supportedBlendModes.end()) {
		SDL_LogError(LC_VIDEO, "DrawContext::setBlendMode() unsupported blend mode");
		return;
	}

    setupBlendFunctions.at(blendmode)();
	curMode = blendmode;
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
void DrawContext::setScissorBox(utils::recti box)
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
	glEnable(GL_BLEND);
	glEnable(GL_PROGRAM_POINT_SIZE);
	glEnable(GL_LINE_SMOOTH);
	
	glPixelStorei(GL_PACK_ALIGNMENT, 1);

	setBlendMode(curMode);

	enableCullFace(curCullFace.enabled);
	setCullMode(curCullFace.mode);

	enableDepthTest(curDepthTest.enabled);
	setDepthFunc(curDepthTest.func);
	setDepthMask(curDepthTest.mask);

	setViewportSize(viewportSize);
}

}
