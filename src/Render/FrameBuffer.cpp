#include "FrameBuffer.h"

namespace render
{

enum ClearBufferFlags : u8
{
	CBF_NONE,
	CBF_COLOR,
	CBF_DEPTH,
	CBF_STENCIL
};

FrameBuffer::FrameBuffer(u32 _width, u32 _height, u32 _maxColorAttachments)
	: width(_width), height(_height), maxColorAttachments(_maxColorAttachments)
{
	colorTextures.resize(maxColorAttachments, nullptr);
	colorCubeMapFaces.resize(maxColorAttachments);

	depthStencilTexture = nullptr;

	glGenFramebuffers(1, &fboID);

	TEST_GL_ERROR();
}

FrameBuffer::~FrameBuffer()
{
	glDeleteFramebuffers(1, &fboID);
}

void FrameBuffer::clearBuffers(u16 flags, img::color8 color, f32 depth, u8 stencil)
{
	GLbitfield mask = 0;

	if (flags & CBF_COLOR) {
		f32 inv = 1.0f / 255.0f;

		glClearColor(color.R() * inv, color.G() * inv, color.B() * inv, color.A() * inv);
		mask |= GL_COLOR_BUFFER_BIT;
	}

	if (flags & CBF_DEPTH) {
		glClearDepthf(depth);
		mask |= GL_DEPTH_BUFFER_BIT;
	}

	if (flags & CBF_STENCIL) {
		glClearStencil(stencil);
		mask |= GL_STENCIL_BUFFER_BIT;
	}

	glClear(mask);
}

void FrameBuffer::setColorTextures(const std::vector<Texture*> &textures, const std::vector<CubeMapFace> &cubeMapFaceMappings)
{
	if (textures.size() == 0)
		return;

	if (textures.size() > maxColorAttachments || cubeMapFaceMappings.size() > maxColorAttachments) {
		WarnStream << "FrameBuffer::setColorTextures() can not attach the textures count" <<
			"higher than GL_MAX_COLOR_ATTACHMENTS, limiting up to the maximum count\n";
	}

	glBindFramebuffer(GL_FRAMEBUFFER, fboID);
	u8 maxTextureCount = std::min((u8)textures.size(), maxColorAttachments);

	colorTextures.resize(maxTextureCount, nullptr);
	colorCubeMapFaces.resize(maxTextureCount);

	for (u8 i = 0; i < maxTextureCount; i++) {
		Texture *tex = textures[i];

		if (colorTextures[i] != nullptr) {
            if (tex->getType() == TT_2D && *tex == *colorTextures[i])
				continue;
			else if (tex->getType() == TT_CUBEMAP && *tex == *colorTextures[i]
				&& cubeMapFaceMappings[i] == colorCubeMapFaces[i])
				continue;
		}

		GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
		GLenum textarget = tex->getType() == TT_2D ? GL_TEXTURE_2D : static_cast<u32>(cubeMapFaceMappings[i]);

		glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, textarget, tex->getID(), 0);

		colorTextures[i] = tex;
		colorCubeMapFaces[i] = cubeMapFaceMappings[i];
	}

#ifdef DEBUG
	checkStatus();
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TEST_GL_ERROR();
}

void FrameBuffer::setDepthStencilTexture(Texture *texture, CubeMapFace dsCubeMapFace)
{
	glBindFramebuffer(GL_FRAMEBUFFER, fboID);

	bool already_attached = false;

	if (depthStencilTexture != nullptr) {
		if (texture->getType() == TT_2D && *texture == *depthStencilTexture)
			already_attached = true;
		else if (texture->getType() == TT_CUBEMAP && *texture == *depthStencilTexture &&
			dsCubeMapFace == depthStencilCubeMapFace)
			already_attached = true;
	}

	if (!already_attached) {
		GLenum target = texture->getType() == TT_2D ? GL_TEXTURE_2D : static_cast<GLenum>(dsCubeMapFace);
#ifdef EMSCRIPTEN
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target, texture->getID(), 0);
#else
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, target, texture->getID(), 0);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, target, texture->getID(), 0);
#endif
		depthStencilTexture = texture;
		depthStencilCubeMapFace = dsCubeMapFace;
	}

#ifdef DEBUG
	checkStatus();
#endif

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TEST_GL_ERROR();
}

bool FrameBuffer::checkStatus() const
{
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

	switch (status) {
		case GL_FRAMEBUFFER_COMPLETE:
			return true;
		case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:
			ErrorStream << "FBO has invalid read buffer\n";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:
			ErrorStream << "FBO has invalid draw buffer\n";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
			ErrorStream << "FBO has one or several incomplete image attachments\n";
			break;
		case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
			ErrorStream << "FBO missing an image attachment\n";
			break;
		case GL_FRAMEBUFFER_UNSUPPORTED:
			ErrorStream << "FBO format unsupported\n";
			break;
		default:
			ErrorStream << "FBO error\n";
			break;
	}

	return false;
}

}
