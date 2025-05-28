#pragma once

#include "Texture2D.h"
#include <list>

namespace render
{

// Texture + PBO for the fast data transfer to GPU providing the minimal delays
// It fits for the situations when the frquent texture updates are necessary like atlases
class StreamTexture2D : public Texture2D {
    u32 pboID;
    u8 *pboData = nullptr;
    GLsync fence = nullptr;

    std::list<rectu> dirtyRegions;
public:
    StreamTexture2D(const std::string &name, u32 width, u32 height,
        img::PixelFormat format, const TextureSettings &settings=TextureSettings());

    ~StreamTexture2D();

    void bind() const override;
    void unbind() const override;

    void uploadSubData(u32 x, u32 y, img::Image *img, img::ImageModifier *imgMod = nullptr) override;

    void flush();
private:
    void mergeRegions(std::list<rectu> &mregions) const;
};

}
