#include "RenderTarget.h"
#include "ITexture.h"
#include "Driver.h"
#include "DrawContext.h"
#include "Common.h"
#include "Logger.h"

#include <stdexcept>

#ifndef GL_FRAMEBUFFER_INCOMPLETE_FORMATS
#define GL_FRAMEBUFFER_INCOMPLETE_FORMATS GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT
#endif

#ifndef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
#define GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT
#endif

namespace video
{

RenderTarget::RenderTarget(IVideoDriver *_driver)
    : driver(_driver)
{
	u8 maxColorAttachments = driver->getFeatures().ColorAttachment;
	colorTextures.resize(maxColorAttachments, nullptr);
	colorCubeMapFaces.resize(maxColorAttachments);

	glGenFramebuffers(1, &fboID);

	driver->testGLError();
}

RenderTarget::~RenderTarget()
{
	glDeleteFramebuffers(1, &fboID);

	for (u8 i = 0; i < colorTextures.size(); i++) {
		if (colorTextures[i])
			colorTextures[i]->drop();
	}

	if (depthStencilTexture)
		depthStencilTexture->drop();

	driver->testGLError();
}

void RenderTarget::bind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	driver->testGLError();
}

void RenderTarget::unbind() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	driver->testGLError();
}

void RenderTarget::setColorTextures(
	const std::vector<ITexture*> &textures,
	const std::vector<E_CUBE_SURFACE> &cubeMapFaceMappings,
	u8 mipLevel)
{
	if (textures.size() == 0)
		return;

	u8 maxColorAttachments = driver->getFeatures().ColorAttachment;
	if (textures.size() > maxColorAttachments || cubeMapFaceMappings.size() > maxColorAttachments) {
		g_irrlogger->log("RenderTarget::setColorTextures() can not attach the textures count"
			"higher than GL_MAX_COLOR_ATTACHMENTS, limiting up to the maximum count", ELL_WARNING);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	driver->testGLError();

	u8 maxTextureCount = std::min((u8)textures.size(), maxColorAttachments);

	for (u8 i = 0; i < maxTextureCount; i++) {
		ITexture *tex = textures[i];

		if (tex) {
			if (colorTextures[i] != nullptr) {
				if (tex->getType() == ETT_2D && tex == colorTextures[i])
					continue;
				else if (tex->getType() == ETT_CUBEMAP && tex == colorTextures[i]
					&& cubeMapFaceMappings[i] == colorCubeMapFaces[i])
					continue;
			}
		}

		if (colorTextures[i]) {
			colorTextures[i]->drop();
			colorTextures[i] = nullptr;
		}

		if (tex) {
			tex->grab();
			colorTextures[i] = tex;

			if (tex->getType() == ETT_CUBEMAP)
				colorCubeMapFaces[i] = cubeMapFaceMappings[i];
		}

		GLuint textureID = 0;
		GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
		GLenum textarget = GL_TEXTURE_2D;

		if (colorTextures[i]) {
			switch (colorTextures[i]->getType()) {
			case ETT_2D:
				textarget = GL_TEXTURE_2D;
				break;
			case ETT_2D_MS:
				textarget = GL_TEXTURE_2D_MULTISAMPLE;
				break;
			case ETT_CUBEMAP:
				textarget = GL_TEXTURE_CUBE_MAP_POSITIVE_X + (int)colorCubeMapFaces[i];
				break;
			default:
				throw std::logic_error("not reachable");
			}

			textureID = (GLuint)colorTextures[i]->getID();
		}

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textarget, textureID, mipLevel);
		driver->testGLError();
	}

	width = colorTextures[0]->getSize().Width;
	height = colorTextures[0]->getSize().Height;

	// Configure drawing operation.

	if (maxColorAttachments > 0) {
		const u8 textureSize = colorTextures.size();

		u8 multipleRenderTargets = driver->getFeatures().MultipleRenderTarget;
		GLenum mode = 0;
		if (textureSize == 0) {
			mode = GL_NONE;
			glDrawBuffers(1, &mode);
		}
		else if (textureSize == 1 || multipleRenderTargets == 0) {
			mode = GL_COLOR_ATTACHMENT0;
			glDrawBuffers(1, &mode);
		}
		else {
			const u32 bufferCount = std::min(multipleRenderTargets, textureSize);

			std::vector<GLenum> colorTexturesEnums;
			colorTexturesEnums.resize(colorTextures.size(), GL_NONE);

			for (u8 i = 0; i < colorTextures.size(); i++) {
				if (colorTextures[i])
					colorTexturesEnums[i] = GL_COLOR_ATTACHMENT0 + i;
			}
			glDrawBuffers(bufferCount, colorTexturesEnums.data());
		}

		driver->testGLError();
	}

#ifdef _DEBUG_
	checkStatus();
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	driver->testGLError();
}

void RenderTarget::setDepthStencilTexture(
	ITexture *texture, E_CUBE_SURFACE dsCubeMapFace, u8 mipLevel)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	driver->testGLError();

	bool already_attached = false;

	if (texture) {
        if (texture->getType() == ETT_2D && texture == depthStencilTexture)
			already_attached = true;
		else if (texture->getType() == ETT_CUBEMAP && texture == depthStencilTexture &&
			dsCubeMapFace == depthStencilCubeMapFace)
			already_attached = true;
	}

	if (!already_attached) {
		if (depthStencilTexture) {
			depthStencilTexture->drop();
			depthStencilTexture = nullptr;
		}

		if (texture) {
			texture->grab();
			depthStencilTexture = texture;

			if (texture->getType() == ETT_CUBEMAP)
				depthStencilCubeMapFace = dsCubeMapFace;
		}

		GLuint textureID = 0;
		GLenum textarget = GL_TEXTURE_2D;

		const ECOLOR_FORMAT textureFormat = depthStencilTexture ? depthStencilTexture->getColorFormat() : ECF_UNKNOWN;

		if (depthStencilTexture) {
			switch (texture->getType()) {
			case ETT_2D:
				textarget = GL_TEXTURE_2D;
				break;
			case ETT_2D_MS:
				textarget = GL_TEXTURE_2D_MULTISAMPLE;
				break;
			case ETT_CUBEMAP:
				textarget = static_cast<GLenum>(dsCubeMapFace);
			default:
				throw std::logic_error("not reachable");
			}

			textureID = depthStencilTexture->getID();
		}

#ifdef _EMSCRIPTEN_
		if (textureFormat == ECF_D24S8) {
			GLenum attachment = 0x821A; // GL_DEPTH_STENCIL_ATTACHMENT
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textarget, textureID, mipLevel);
		}
		else
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textarget, textureID, mipLevel);
#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textarget, textureID, mipLevel);

		if (textureFormat == ECF_D24S8)
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, textarget, textureID, mipLevel);
#endif
		driver->testGLError();
	}

#ifdef _DEBUG_
	checkStatus();
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	driver->testGLError();
}

void RenderTarget::blitTo(RenderTarget *target)
{
	auto driverGL3 = static_cast<COpenGL3DriverBase *>(driver);
	auto version = driverGL3->getVersionFromOpenGL();
	if (version.Spec == OpenGLSpec::ES && version.Major < 3) {
		g_irrlogger->log("glBlitFramebuffer not supported by OpenGL ES < 3.0", ELL_ERROR);
		return;
	}

	auto prev_rt = driverGL3->getContext()->getRenderTarget();

	glBindFramebuffer(GL_READ_FRAMEBUFFER, getID());
	driver->testGLError();
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, target->getID());
	driver->testGLError();
	glBlitFramebuffer(
			0, 0, getSize().Width, getSize().Height,
			0, 0, target->getSize().Width, target->getSize().Height,
			GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, GL_NEAREST);
	driver->testGLError();

	// This resets both read and draw framebuffer. Note that we bypass CacheHandler here.
	glBindFramebuffer(GL_FRAMEBUFFER, prev_rt->getID());
	driver->testGLError();
}

bool RenderTarget::checkStatus() const
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	driver->testGLError();

	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE:
			return true;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			g_irrlogger->log("FBO has invalid read buffer", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			g_irrlogger->log("FBO has invalid draw buffer", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			g_irrlogger->log("FBO has one or several incomplete image attachments", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_FORMATS:
			g_irrlogger->log("FBO has one or several image attachments with different internal formats", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
			g_irrlogger->log("FBO has one or several image attachments with different dimensions", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			g_irrlogger->log("FBO missing an image attachment", ELL_ERROR);
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			g_irrlogger->log("FBO format unsupported", ELL_ERROR);
			break;
		default:
			g_irrlogger->log("FBO error", ELL_ERROR);
			break;
	}

	return false;
}

}
