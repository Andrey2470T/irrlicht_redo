#include "StreamTexture2D.h"

namespace render
{

StreamTexture2D::StreamTexture2D(const std::string &name, u32 width, u32 height,
    img::PixelFormat format, const TextureSettings &settings)
    : Texture2D(name, std::make_unique<img::Image>(format, width, height), settings)
{
    glGenBuffers(1, &pboID);
    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);

    auto pixelSize = img::pixelFormatInfo.at(format).size;
    glBufferStorage(GL_PIXEL_UNPACK_BUFFER, width * height * pixelSize, nullptr,
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    pboData = (u8*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, width * height * pixelSize,
        GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

    TEST_GL_ERROR();
}

StreamTexture2D::~StreamTexture2D()
{
    glDeleteBuffers(1, &pboID);
    glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);

    TEST_GL_ERROR();
}

void StreamTexture2D::bind() const
{
    Texture2D::bind();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, pboID);
}

void StreamTexture2D::unbind() const
{
    Texture2D::unbind();

    glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
}

void StreamTexture2D::uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod)
{
    if (img->getFormat() != format) {
        ErrorStream << "StreamTexture2D::uploadSubData() the format of the uploaded image data is different\n";
        return;
    }

    v2u pos = img->getClipPos();
    v2u size = img->getClipSize();
    auto pixelSize = img::pixelFormatInfo.at(format).size;

    if (imgMod) {
        rectu srcRect(pos.X, pos.Y, size.X, size.Y);
        rectu destRect(x, y, size.X, size.Y);
        imgMod->copyTo(img, imgCache.get(), &srcRect, &destRect);
    }
    v2u imgSize = img->getSize();
    memcpy(pboData + (y * imgSize.X + x) * pixelSize, img->getData(), size.X * size.Y * pixelSize);

    dirtyRegions.emplace_back(x, y, size.X, size.Y);
}

void StreamTexture2D::flush()
{
    if (dirtyRegions.empty())
        return;

    if (fence) {
        glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
        glDeleteSync(fence);
    }

    std::list<rectu> mergedRegions;
    mergeRegions(mergedRegions);

    auto formatInfo = img::pixelFormatInfo.at(format);
    for (auto &region : mergedRegions) {
        u32 x = region.ULC.X;
        u32 y = region.ULC.Y;
        u32 width = region.getWidth();
        u32 height = region.getHeight();
        u32 offset = (y * width + x) * formatInfo.size;
        u32 length = width * height * formatInfo.size;
        glFlushMappedBufferRange(GL_PIXEL_UNPACK_BUFFER, offset, length);

        glTexSubImage2D(GL_TEXTURE_2D, 0, x, y, width, height, formatInfo.pixelFormat, formatInfo.pixelType, nullptr);
    }

    fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    dirtyRegions.clear();
}

void StreamTexture2D::mergeRegions(std::list<rectu> &mregions) const
{
    if (dirtyRegions.empty())
        return;

    for (auto &region : dirtyRegions) {
        auto rsize = region.getSize();

        bool merged = false;
        for (auto &mregion : mregions) {
            auto mrsize = mregion.getSize();
            v2u dist = region.getCenter() - mregion.getCenter();

            if (dist.X == (rsize.X/2 + mrsize.X/2) &&
                dist.Y == (rsize.Y/2 + mrsize.Y/2)) {
                mregion.addInternalPoint(region.ULC);
                mregion.addInternalPoint(region.LRC);
                merged = true;
                break;
            }

            if (!merged)
                mregions.emplace_back(region);
        }
    }
}

}
