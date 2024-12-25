#include "FrameBuffer.h"

namespace render
{

FrameBuffer(u32 _width, u32 _height, u32 _maxColorAttachments)
	: width(_width), height(_height), maxColorAttachments(_maxColorAttachments)
{
	colorTextures.resize(maxColorAttachments, nullptr);

	glGenFramebuffers(1, &fboID);

	TEST_GL_ERROR();
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fboID);
}

void FrameBuffer::setColorTextures(const std::vector<Texture*> &textures, const std::vector<CubeMapFace> &cubeMapFaceMappings)
{
	if (textures.size() == 0)
		return;

	if (textures.size() > maxColorAttachments || cubeMapFaceMappings.size() > maxColorAttachments) {
		SDL_LogWarn(LC_VIDEO, "FrameBuffer::setColorTextures() can not attach the textures count\
			higher than GL_MAX_COLOR_ATTACHMENTS, limiting up to the maximum count");
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	u8 maxTextureCount = std::min(textures.size(), maxColorAttachments);

	colorTextures.resize(maxColorAttachments, nullptr);

	for (u8 i = 0; i < maxTextureCount; i++) {
		Texture *tex = textures[i];
		GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
		GLenum textarget = tex->getType() == TT_2D ? GL_TEXTURE_2D : static_cast<u32>(cubeMapFaceMappings[i]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textarget, tex->getID(), 0);

		colorTextures[i] = tex;
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TEST_GL_ERROR();
}

void FrameBuffer::setDepthStencilTexture(Texture *texture, CubeMapFace dsCubeMapFace)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
		texture->getType() == TT_2D ? GL_TEXTURE_2D : static_cast<u32>(dsCubeMapFace), texture->getID(), 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TEST_GL_ERROR();
}

}
