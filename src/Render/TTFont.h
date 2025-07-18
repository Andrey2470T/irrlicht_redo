#pragma once

#include "Image/Image.h"
#include <SDL_ttf.h>

namespace render
{

enum class FontMode : u8
{
    MONO        = 0x00,
    GRAY          = 0x01,
    FALLBACK = 0x02
};

enum class FontStyle : u8
{
    NORMAL                   = 0x00,
    BOLD                         = 0x01,
    ITALIC                       = 0x02,
    UNDERLINE              = 0x04,
    STRIKETHROUGH   = 0x08
};

class TTFont
{
    static bool isInit;

    TTF_Font *font;

    FontMode mode = FontMode::MONO;
    FontStyle style = FontStyle::NORMAL;
    u32 curSize;
    bool hasTransparency = true;
    u32 shadowOffset;
    u32 shadowAlpha;
public:
    TTFont(TTF_Font *_font, FontMode _mode, u32 _size, bool _transparent, u32 _shadow_offset, u32 _shadow_alpha);

    ~TTFont();

    static bool init();
    static void free();
    static TTFont *load(const std::string &path, u32 size, u32 face=0, bool antialias=true,
        bool transparent=true, u32 shadow_offset=0, u32 shadow_alpha=255);
    static TTFont *loadFromMem(void *mem, u32 size, u32 face=0, bool antialias=true,
        bool transparent=true, u32 shadow_offset=0, u32 shadow_alpha=255);

    FontMode getMode() const;
    FontStyle getStyle() const;

    u32 getTextWidth(const std::wstring &text) const;
    u32 getTextHeight(const std::wstring &text) const;
    v2u getTextSize(const std::wstring &text) const;
    u32 getLineHeight() const;

    u32 getFontHeight() const;
    u32 getKerningSizeForTwoChars(wchar_t ch1, wchar_t ch2) const;

    s32 getFontAscent() const;
    s32 getFontDescent() const;
    void getGlyphMetrics(wchar_t ch, s32 *offsetx, s32 *offsety, s32 *advance) const;

    u32 getCurrentSize() const;
    bool isTransparent() const;

    void getShadowParameters(u32 *offset, u32 *alpha) const
    {
        *offset = shadowOffset;
        *alpha = shadowAlpha;
    }

    std::optional<u32> getCharFromPos(const std::wstring &str, s32 pixel_x) const;
    
    bool hasGlyph(wchar_t c) const;

    void setSize(u32 size);

    img::Image *getGlyphImage(wchar_t ch, const img::color8 &char_color=img::color8(img::PF_RGBA8, 255, 255, 255, 255));

    img::Image *drawText(const std::wstring &text, const img::color8 &color=img::color8(img::PF_RGBA8, 255, 255, 255, 255));

    static u64 hash(const TTFont *font);
    static u64 hash(u32 size, bool transparent, FontStyle style, FontMode mode);
    bool operator==(const TTFont *other) const;
};

}
